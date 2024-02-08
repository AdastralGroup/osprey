#include <kachemak/kachemak.hpp>

Kachemak::Kachemak(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
                   const std::string& szSourceUrl, const std::filesystem::path& ButlerPath)
    : Version(szSourcemodPath, szFolderName, szSourceUrl) {
  // placeholder
  m_szTempPath = std::filesystem::temp_directory_path().string();
  name = szFolderName.string(); // this is bad don't do this
  m_szButlerLocation = ButlerPath;
  std::string ver_string = net().get_string_data_from_server(szSourceUrl + "bullseye.json");
  if(!nlohmann::json::accept(ver_string)){
    throw std::runtime_error("INVALID JSON. \n " + szSourceUrl + "bullseye.json");
  }
  m_parsedVersion = nlohmann::ordered_json::parse(ver_string);
  FindInstalledVersion();
  m_eventSystem.RegisterListener(EventType::kOnUpdate, [](Event& ev) {
    double prog = ((ProgressUpdateMessage&)ev).GetProgress();
    //A_printf("[Kachemak/Butler] Progress: %f\n", round(prog*100));
  });
}

std::optional<KachemakVersion> Kachemak::GetKMVersion(const std::string& version) {
  nlohmann::ordered_json& jsonVersion = m_parsedVersion["versions"][version];
  if (!jsonVersion.is_object()) {
    A_printf("[Kachemak/GetKMVersion] Failed to find version %s\n", version.c_str());
    return std::nullopt;
  }

  KachemakVersion ret = {
      .szFileName = jsonVersion["file"].get<std::string>(),
//      .szDownloadUrl = jsonVersion["url"].get<std::string>(),
      .szDownloadUrl = jsonVersion["file"].get<std::string>(),
//      .lDownloadSize = jsonVersion["presz"].get<std::size_t>(),
//      .lExtractSize = jsonVersion["postsz"].get<std::size_t>(),
      .szVersion = version,
      .szSignature = jsonVersion["signature"].get<std::string>(),
  };

  return ret;
}

std::optional<KachemakPatch> Kachemak::GetPatch(const std::string& version) { //this doesn't work if there's missing fields
  nlohmann::ordered_json& jsonPatches = m_parsedVersion["patches"][version];
  if (!jsonPatches.is_object()) {
    A_printf("[Kachemak/GetPatch] Failed to find patch %s\n", version.c_str());
    return std::nullopt;
  }

  KachemakPatch ret = {
      .szUrl = jsonPatches["url"].get<std::string>(),
      .szFilename = jsonPatches["file"].get<std::string>(),
      .lTempRequired = jsonPatches["tempreq"].get<std::uintmax_t>(),
  };

  return ret;
}

std::optional<KachemakVersion> Kachemak::GetLatestKMVersion() {
  std::string versionId;
  for (auto& el : m_parsedVersion["versions"].items()) {
    versionId = el.key();
  };

  return GetKMVersion(versionId);
}

/*
description:
  check free space for specific category provided.
res:
  0: success
  1: not enough temp storage
  2: not enough permanent storage
*/
int Kachemak::FreeSpaceCheck(const uintmax_t size, const FreeSpaceCheckCategory& category) {
  switch (category) {
    case FreeSpaceCheckCategory::Temporary:
      if (std::filesystem::space(m_szTempPath).free < size) {
        return 1;
      }
      break;
    case FreeSpaceCheckCategory::Permanent:
      if (std::filesystem::space(m_szSourcemodPath).free < size) {
        return 2;
      }
      break;
  }
  return 0;
}

int Kachemak::PrepareSymlink() {
#ifdef _WIN32
  return 0;
#else
  for (const auto& item : TO_SYMLINK) {
    const std::filesystem::path symlinkPath = m_szSourcemodPath / item[1];
    if (std::filesystem::exists(symlinkPath) && !std::filesystem::is_symlink(symlinkPath)) {
      std::filesystem::remove(symlinkPath);
    }
  }

  return 0;
#endif
}

int Kachemak::DoSymlink() {
#ifdef _WIN32
  return 0;
#else
  return 0; //leave this for now
  for (const auto& item : TO_SYMLINK) {
    const std::filesystem::path symlinkPath = m_szSourcemodPath / m_szFolderName / item[1];
    if (!std::filesystem::exists(symlinkPath)) {
      std::filesystem::create_symlink(m_szSourcemodPath / item[0], symlinkPath);
    }
  }

  return 0;
#endif
}


int Kachemak::Verify(){
  std::optional<KachemakVersion> installedKMVersion = GetKMVersion(m_szInstalledVersion);
  if (!installedKMVersion) return 4;
  // full signature url
  std::stringstream sigUrlFull_ss;
  sigUrlFull_ss << m_szSourceUrl << installedKMVersion.value().szSignature;
  // Data path for current install
  std::filesystem::path dataDir_path = m_szSourcemodPath / m_szFolderName;
  std::stringstream healUrl_ss;
  healUrl_ss << m_szSourceUrl << installedKMVersion.value().szFileName;
  int verifyRes = ButlerVerify(sigUrlFull_ss.str(), dataDir_path.string(), healUrl_ss.str());
  force_verify = false;
  WriteVersion();
  return 0;
}


/*
description:
  routine for updating whatever is installed
res:
  0: success
  1: symlink fail
  2: space check fail
  3: already on latest / verification needed first
*/

int Kachemak::Update() {
  A_printf("[Kachemak/Update] Updating %s... ",m_szFolderName.c_str());
  if (m_szInstalledVersion == GetLatestVersion() || force_verify){
    return 3;
  }
  int symlinkRes = PrepareSymlink();
  if (symlinkRes != 0) {
    return 1;
  }

  std::optional<KachemakPatch> patch = GetPatch(m_szInstalledVersion);
  if (!patch) return 3;

  if (FreeSpaceCheck(patch.value().lTempRequired, FreeSpaceCheckCategory::Permanent) != 0) {
    return 2;
  }

  std::optional<KachemakVersion> installedVersion = GetKMVersion(m_szInstalledVersion);
  if (!installedVersion) return 4;

  // full signature url
  std::stringstream sigUrlFull_ss;
  sigUrlFull_ss << m_szSourceUrl << installedVersion.value().szSignature;
  // Data path for current install
  std::filesystem::path dataDir_path = m_szSourcemodPath / m_szFolderName;
  std::stringstream healUrl_ss;
  healUrl_ss << m_szSourceUrl << installedVersion.value().szFileName;
  int verifyRes = ButlerVerify(sigUrlFull_ss.str(), dataDir_path.string(), healUrl_ss.str());

  std::stringstream patchUrlFull_ss;
  patchUrlFull_ss << m_szSourceUrl << patch.value().szUrl;
  std::filesystem::path stagingPath = m_szSourcemodPath / ("butler-staging-" + m_szFolderName.string()); // make this dynamic, so we can download multiple games at once
  A_printf("[Kachemak/Update] Patching %s from %s to %s, with staging dir at %s. ",m_szFolderName.c_str(),installedVersion.value().szVersion.c_str(),GetLatestVersion().c_str(),stagingPath.c_str());
  int patchRes = ButlerPatch(patchUrlFull_ss.str(), stagingPath.string(), patch.value().szFilename,
                             dataDir_path.string(), patch.value().lTempRequired);

  DoSymlink();
  m_szInstalledVersion = GetLatestVersion();
  WriteVersion();
  return 0;
}

int Kachemak::Install() {
  if (PrepareSymlink() != 0) {
    return 1;
  }
  std::optional<KachemakVersion> latestVersion = GetLatestKMVersion();
  if (!latestVersion) return 2;
  int diskSpaceStatus = FreeSpaceCheck(latestVersion.value().lDownloadSize, FreeSpaceCheckCategory::Temporary);
  if (diskSpaceStatus != 0) return diskSpaceStatus;
  std::string downloadUri = m_szSourceUrl + latestVersion.value().szDownloadUrl;
  A_printf("[Kachemak/Install] Downloading from fusion...\n");
  //int downloadStatus =
  std::filesystem::path path = net::download_to_temp(downloadUri, latestVersion.value().szFileName, true,&m_eventSystem);
  //if (downloadStatus != 0) {
  //  A_printf("[Kachemak/Install] Download failed - ret val %d \n",downloadStatus);
  //  return downloadStatus;
  //}
  A_printf("[Kachemak/Install] Download complete: extracting... \n");
  std::filesystem::create_directory(m_szSourcemodPath.string() / m_szFolderName);
  Extract( path.string() , (m_szSourcemodPath/ m_szFolderName).string() , latestVersion.value().lExtractSize);
  A_printf("[Kachemak/Install] Extraction done.... \n");
  DoSymlink();
  m_szInstalledVersion = GetLatestVersion();
  WriteVersion();
  return 0;
}

/*
desc:
        extract .zip file to directory
res:
        0: success
        1: not enough free space
*/
int Kachemak::Extract(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize) {
  A_printf("[Kachemak/Extract] Input File %s, Output %s, size %lu \n",szInputFile.c_str(),szOutputDirectory.c_str(),szSize);
  if (FreeSpaceCheck(szSize * 2, FreeSpaceCheckCategory::Permanent) != 0) {
    A_printf("[Kachemak/Extract] Not enough space. Exiting. \n");
    return 1;
  }
  std::FILE* tmpf = std::tmpfile();
  std::string tmpf_loc = std::to_string(fileno(tmpf));
  int ret = sys::ExtractZip(szInputFile, szOutputDirectory);
  if (ret != 0) {
    A_printf("[Kachemak/Extract] Extraction Failed - %s\n",zip_strerror(ret));
  }
  m_szInstalledVersion = GetLatestVersion();
  WriteVersion();
  return 0;
}

/*
description:
  wrapper for butler cli to verify installation
res:
  0: success
  i: return code from butler (when not 0)
*/
int Kachemak::ButlerVerify(const std::string& szSignature, const std::string& szGameDir, const std::string& szRemote) {
  std::stringstream params;

  // {m_butlerLoation} verify {szSignature} {szGameDir} --heal=archive{szRemote}
  params << m_szButlerLocation.string() << " verify " << szSignature.c_str() << " "
         << "\"" << szGameDir.c_str() << "\""
         << " "
         << "--heal=archive," << szRemote << " --json";
  A_printf(szSignature.c_str());
  int status = ButlerParseCommand(params.str());
  if (status != NULL) return status;

  return 0;
}
/*
description:
  patch installation
res:
  0: success
  1: failed to download patch
  2: failed to apply patch*/
int Kachemak::ButlerPatch(const std::string& sz_url, const std::filesystem::path& sz_stagingDir,
                          const std::string& sz_patchFileName, const std::string& sz_gameDir,
                          const uintmax_t downloadSize) {
  bool stagingDir_exists = std::filesystem::exists(sz_stagingDir);
  bool stagingDir_isDir = std::filesystem::is_directory(sz_stagingDir);
  if (!stagingDir_exists) {
    if (!std::filesystem::create_directory(sz_stagingDir)) {
      A_printf("[Kachemak/ButlerPatch] Failed to create directory: %s \n",sz_stagingDir.c_str());
    }
  }
  if (stagingDir_exists && stagingDir_isDir) {
    switch (sys::DeleteDirectoryContent(sz_stagingDir)) {
      case 1:
        A_printf("[Kachemak/ButlerPatch] Failed to delete staging directory content "
                     "(doesn't exist)\n");
        break;
      case 2:
        A_printf("[Kachemak/ButlerPatch] Failed to delete staging directory content "
                     "(not a directory)\n");
        break;
    }
  }

  int diskSpaceStatus = FreeSpaceCheck(downloadSize, FreeSpaceCheckCategory::Temporary);
  if (diskSpaceStatus != 0) return diskSpaceStatus;
  //int downloadStatus = bilsdale::LibTorrentDownload(sz_url, m_szTempPath.string());
  net::download_to_temp(sz_url, sz_patchFileName, true,&m_eventSystem,&m_szTempPath);
  //if (downloadStatus != 0) return downloadStatus;

  std::filesystem::path tempPath = m_szTempPath / sz_patchFileName;

  std::stringstream params;

  params << m_szButlerLocation.string() << " ";
  params << "apply"
         << " ";
  params << "--staging-dir=\"" << sz_stagingDir.string() << "\""
         << " ";
  params << "\"" << tempPath.string() << "\""
         << " ";
  params << "\"" << sz_gameDir << "\""
         << " ";
  params << "--json";

  A_printf("[Kachemak/ButlerPatch] Applying patch.\n");
  if (int butlerStatus = ButlerParseCommand(params.str()); butlerStatus != NULL) {
    A_printf("[Kachemak/ButlerPatch] Failed to apply patch: %s\n",butlerStatus);
    return 2;
  }

  switch (sys::DeleteDirectoryContent(sz_stagingDir)) {
    case 1:
      A_printf("[Kachemak/ButlerPatch] Failed to delete staging directory content "
                   "(doesn't exist)\n");
      break;
    case 2:
      A_printf("[Kachemak/ButlerPatch] Failed to delete staging directory content "
                   "(not a directory)\n");
      break;
  }
  return 0;
}

int Kachemak::ButlerParseCommand(const std::string& command) {
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    A_printf("Failed to create pipe for butler verification\n");
    return 1;
  }

  const uint16_t bufSize = 4096;
  char buffer[bufSize];
  while (fgets(buffer, bufSize, pipe)) {
    nlohmann::json jsonBuffer = nlohmann::json::parse(buffer, nullptr, false);
    if (jsonBuffer.is_discarded()) continue;

    if (jsonBuffer.contains("type")) {
      std::string messageType = jsonBuffer["type"].get<std::string>();

      if (messageType.compare("progress") == NULL) {
        ProgressUpdateMessage message(jsonBuffer["bps"].get<float>(), jsonBuffer["progress"].get<float>());
        m_eventSystem.TriggerEvent(message);
      } else if (messageType.compare("error") == NULL) {
        ErrorMessage message(jsonBuffer["message"].get<std::string>());
        m_eventSystem.TriggerEvent(message);
      }
    }
  }

  pclose(pipe);

  return 0;
}
void Kachemak::FindInstalledVersion() {
  if (exists(m_szSourcemodPath / m_szFolderName)){
    std::ifstream data(m_szSourcemodPath / m_szFolderName / ".adastral");
    if (!data.fail()) {
      nlohmann::json filedata = nlohmann::json::parse(data);
      m_szInstalledVersion = filedata["version"];
      A_printf("[Kachemak/InstalledVersion] version: %s\n",m_szInstalledVersion.c_str());
    }else{
      A_printf("[Kachemak/InstalledVersion] Adastral supported game detected (%s), but .adastral not detected."
          "Assuming best case and setting force_version.\n",m_szFolderName.c_str());
      m_szInstalledVersion = GetLatestVersion();
      force_verify = true;
    }
  }else{
    A_printf("[Kachemak/InstalledVersion] Game not installed. Booo.");
  }
}
std::string Kachemak::GetLatestVersion() {
  std::string versionId;
  for (auto& el : m_parsedVersion["versions"].items()) {
    versionId = el.key();
  };
  return versionId;
}

void Kachemak::WriteVersion(){
  nlohmann::json test_json;
  test_json["version"] = m_szInstalledVersion;
  std::ofstream data(m_szSourcemodPath / m_szFolderName / ".adastral");
  data << test_json;
  data.close();
}