#include <events/error.hpp>
#include <events/progress.hpp>
#include <kachemak/kachemak.hpp>
#include "sheffield/sheffield.hpp"

Kachemak::Kachemak(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
                   const std::string& szSourceUrl, const std::string installed_version)
    : Version(szSourcemodPath, szFolderName, szSourceUrl) {
  // placeholder
  m_szTempPath = std::filesystem::temp_directory_path().string();
  name = szFolderName; // this is bad don't do this
  //m_szButlerLocation = std::filesystem::current_path() / "bin" / "butler";
  //m_szAria2cLocation = std::filesystem::current_path() / "bin" / "aria2c";
  m_szButlerLocation = std::filesystem::path("/usr/bin/butler");
  m_szAria2cLocation = std::filesystem::path("/bin/aria2c");
  m_szInstalledVersion = installed_version; // dumb dumb stupid
  std::string ver_string = moss().get_string_data_from_server(szSourceUrl+"/versions.json");
  if(!nlohmann::json::accept(ver_string)){
    throw std::runtime_error("INVALID JSON YOU IDIOT!!!!!!");
  }
  m_parsedVersion = nlohmann::json::parse(ver_string);
  std::cout << m_parsedVersion["patches"] << std::endl;
  m_eventSystem.RegisterListener(EventType::kOnUpdate, [](Event& ev) { printf("Progress: %f\n", ((ProgressUpdateMessage&)ev).GetProgress()); });
}

std::optional<KachemakVersion> Kachemak::GetVersion(const std::string& version) {
  nlohmann::json& jsonVersion = m_parsedVersion["versions"][version];
  if (!jsonVersion.is_object()) {
    printf("Failed to find patch %s\n", version.c_str());
    return std::nullopt;
  }

  KachemakVersion ret = {
      .szFileName = jsonVersion["file"].get<std::string>(),
      .szDownloadUrl = jsonVersion["url"].get<std::string>(),
      .lDownloadSize = jsonVersion["presz"].get<std::size_t>(),
      .lExtractSize = jsonVersion["postsz"].get<std::size_t>(),
      .szVersion = version,
      .szSignature = jsonVersion["signature"].get<std::string>(),
  };

  return ret;
}

std::optional<KachemakPatch> Kachemak::GetPatch(const std::string& version) {
  nlohmann::json& jsonPatches = m_parsedVersion["patches"][version];
  if (!jsonPatches.is_object()) {
    printf("Failed to find patch %s\n", version.c_str());
    return std::nullopt;
  }

  KachemakPatch ret = {
      .szUrl = jsonPatches["url"].get<std::string>(),
      .szFilename = jsonPatches["file"].get<std::string>(),
      .lTempRequired = jsonPatches["tempreq"].get<std::uintmax_t>(),
  };

  return ret;
}

std::optional<KachemakVersion> Kachemak::GetLatestVersion() {
  std::string versionId;
  for (auto& el : m_parsedVersion["versions"].items()) {
    versionId = el.key();
  };

  return GetVersion(versionId);
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
/*
description:
  routine for updating whatever is installed
res:
  0: success
  1: symlink fail
  2: space check fail
*/
int Kachemak::Update() {
  int symlinkRes = PrepareSymlink();
  if (symlinkRes != 0) {
    return 1;
  }

  std::optional<KachemakPatch> patch = GetPatch(m_szInstalledVersion);
  if (!patch) return 3;

  if (FreeSpaceCheck(patch.value().lTempRequired, FreeSpaceCheckCategory::Permanent) != 0) {
    return 2;
  }

  std::optional<KachemakVersion> installedVersion = GetVersion(m_szInstalledVersion);
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
  std::filesystem::path stagingPath = m_szSourcemodPath / "butler-staging";
  int patchRes = ButlerPatch(patchUrlFull_ss.str(), stagingPath.string(), patch.value().szFilename,
                             dataDir_path.string(), patch.value().lTempRequired);

  DoSymlink();
  return 0;
}

int Kachemak::Install() {
  if (PrepareSymlink() != 0) {
    return 1;
  }

  std::optional<KachemakVersion> latestVersion = GetLatestVersion();
  if (!latestVersion) return 2;
  int diskSpaceStatus = FreeSpaceCheck(latestVersion.value().lDownloadSize, FreeSpaceCheckCategory::Temporary);
  if (diskSpaceStatus != 0) return diskSpaceStatus;
  std::string downloadUri = m_szSourceUrl + latestVersion.value().szDownloadUrl;
  int downloadStatus = sheffield(m_szAria2cLocation).AriaDownload(downloadUri, m_szSourcemodPath);
  if (downloadStatus != 0) return downloadStatus;

  Extract(latestVersion.value().szFileName, m_szSourcemodPath.string(), latestVersion.value().lExtractSize);
  DoSymlink();
  return 0;
}

/*
desc:
        extract .zip file to directory
res:
        0: success
        1: not enough free space
        2: failed to delete tmp file
        11: failed to open input file
        12: failed to create zstd context
        13: failed to allocate memory
        14: failed to decompress
        21: failed to open tar file
*/
int Kachemak::Extract(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize) {
  if (FreeSpaceCheck(szSize * 2, FreeSpaceCheckCategory::Permanent) != 0) {
    return 1;
  }
  std::FILE* tmpf = std::tmpfile();
  std::string tmpf_loc = std::to_string(fileno(tmpf));
  moss::ExtractZip(szInputFile, szOutputDirectory);
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
      std::cerr << "[ButlerPatch] Failed to create directory: " << sz_stagingDir.string() << std::endl;
    }
  }
  if (stagingDir_exists && stagingDir_isDir) {
    switch (moss::DeleteDirectoryContent(sz_stagingDir)) {
      case 1:
        std::cerr << "[ButlerPatch] Failed to delete staging directory content "
                     "(doesn't exist)";
        break;
      case 2:
        std::cerr << "[ButlerPatch] Failed to delete staging directory content "
                     "(not a directory)";
        break;
    }
  }

  int diskSpaceStatus = FreeSpaceCheck(downloadSize, FreeSpaceCheckCategory::Temporary);
  if (diskSpaceStatus != 0) return diskSpaceStatus;
  int downloadStatus = sheffield(m_szAria2cLocation).AriaDownload(sz_url, m_szTempPath);
  if (downloadStatus != 0) return downloadStatus;

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

  std::cout << "[ButlerPatch] Applying patch" << std::endl;
  int butlerStatus = ButlerParseCommand(params.str());
  if (butlerStatus != NULL) {
    std::cerr << "[ButlerPatch] Failed to apply patch: " << butlerStatus << std::endl;
    return 2;
  }

  switch (moss::DeleteDirectoryContent(sz_stagingDir)) {
    case 1:
      std::cerr << "[ButlerPatch] Failed to delete staging directory content "
                   "(doesn't exist)";
      break;
    case 2:
      std::cerr << "[ButlerPatch] Failed to delete staging directory content "
                   "(not a directory)";
      break;
  }
  return 0;
}

int Kachemak::ButlerParseCommand(const std::string& command) {
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    printf("Failed to create pipe for butler verification\n");
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
