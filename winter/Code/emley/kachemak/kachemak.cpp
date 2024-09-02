#include <kachemak/kachemak.hpp>

Kachemak::Kachemak(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
                   const std::string& szSourceUrl, const std::filesystem::path& ButlerPath) :
    Version(szSourcemodPath, szFolderName, szSourceUrl)
{
  // placeholder
  m_szTempPath = std::filesystem::temp_directory_path().string();
  name = szFolderName.string();  // this is bad don't do this
  m_szButlerLocation = ButlerPath;
  std::string ver_string = net().get_string_data_from_server(szSourceUrl + "bullseye.json");
  if(!nlohmann::json::accept(ver_string))
  {
    throw std::runtime_error("INVALID JSON. \n " + szSourceUrl + "bullseye.json");
  }
  m_parsedVersion = nlohmann::ordered_json::parse(ver_string);
  find_installed_version();
  m_eventSystem.RegisterListener(EventType::kOnUpdate,
                                 [](Event& ev)
                                 {
                                   // long double prog = ((ProgressUpdateMessage&)ev).GetProgress();
                                   // long double prog2 = ((ProgressUpdateMessage&)ev).GetProgress();
                                   // A_printf("[Kachemak/Butler] Progress: %d (unrounded: %f)\n",
                                   // round(prog2*100),prog);
                                 });
}

std::optional<KachemakVersion> Kachemak::get_km_version(const std::string& version)
{
  nlohmann::ordered_json& jsonVersion = m_parsedVersion["versions"][version];
  if(!jsonVersion.is_object())
  {
    A_printf("[Kachemak/get_km_version] Failed to find version %s", version.c_str());
    return std::nullopt;
  }

  KachemakVersion ret = {
    .szFileName = jsonVersion["file"].get<std::string>(),
    //      .szDownloadUrl = jsonVersion["url"].get<std::string>(),
    .szDownloadUrl = jsonVersion["file_p2p"].get<std::string>(),
    //      .lDownloadSize = jsonVersion["presz"].get<std::size_t>(),
    //      .lExtractSize = jsonVersion["postsz"].get<std::size_t>(),
    .szVersion = version,
    .szSignature = jsonVersion["sig"].get<std::string>(),
  };

  return ret;
}

std::optional<KachemakPatch> Kachemak::get_patch(const std::string& version)
{  // this doesn't work if there's missing fields
  std::string latest = m_parsedVersion["latest"];
  nlohmann::ordered_json& jsonPatches = m_parsedVersion["patches"][latest][version];
  if(!jsonPatches.is_object())
  {
    A_printf("[Kachemak/get_patch] Failed to find patch %s", version.c_str());
    return std::nullopt;
  }

  KachemakPatch ret = {
    .szUrl = jsonPatches["file_p2p"].get<std::string>(),
    .szFilename = jsonPatches["file"].get<std::string>(),
    .lTempRequired = jsonPatches["tempreq"].get<std::uintmax_t>(),
  };

  return ret;
}

std::optional<KachemakVersion> Kachemak::get_latest_km_version() { return get_km_version(m_parsedVersion["latest"]); }

/*
description:
  check free space for specific category provided.
res:
  0: success
  1: not enough temp storage
  2: not enough permanent storage
*/
int Kachemak::free_space_check(const uintmax_t size, const FreeSpaceCheckCategory& category)
{
  switch(category)
  {
    case FreeSpaceCheckCategory::Temporary:
      if(std::filesystem::space(m_szTempPath).free < size)
      {
        return 1;
      }
      break;
    case FreeSpaceCheckCategory::Permanent:
      if(std::filesystem::space(m_szSourcemodPath).free < size)
      {
        return 2;
      }
      break;
  }
  return 0;
}

int Kachemak::free_space_check_path(const uintmax_t size, const std::filesystem::path customPath)
{
  if(std::filesystem::space(customPath).free < size)
  {
    return 1;
  }
  return 0;
}

int Kachemak::prepare_symlink()
{
#ifdef _WIN32
  return 0;
#else
  for(const auto& item : TO_SYMLINK)
  {
    const std::filesystem::path symlinkPath = m_szSourcemodPath / item[1];
    if(std::filesystem::exists(symlinkPath) && !std::filesystem::is_symlink(symlinkPath))
    {
      std::filesystem::remove(symlinkPath);
    }
  }

  return 0;
#endif
}

int Kachemak::do_symlink()
{
#ifdef _WIN32
  return 0;
#else
  return 0;  // leave this for now
  for(const auto& item : TO_SYMLINK)
  {
    const std::filesystem::path symlinkPath = m_szSourcemodPath / m_szFolderName / item[1];
    if(!std::filesystem::exists(symlinkPath))
    {
      std::filesystem::create_symlink(m_szSourcemodPath / item[0], symlinkPath);
    }
  }

  return 0;
#endif
}

int Kachemak::do_symlink_path(std::filesystem::path customPath)
{
#ifdef _WIN32
  // Abandon all hope, ye who enter there, for I am pulling the ancient arts of Windows API.
  // So ancient, even if the IDE complains about the strings it actually works just fine.

  for(const auto& item : TO_SYMLINK)
  {
    // First, get the path and check if it would even exist
    const std::filesystem::path symlinkPath = m_szSourcemodPath / m_szFolderName / item[1];

    // Then check if they even or not. If they don't exist in the path itself, then one can create them.
    if(!std::filesystem::exists(symlinkPath))
    {
      // Do the symbolic link through CreateSymbolicLink function.
      int errorCode = CreateSymbolicLinkW(symlinkPath.wstring().c_str(), customPath.wstring().c_str(),
                                          SYMBOLIC_LINK_FLAG_DIRECTORY);

      // Hopefully it should work just fine, but if it can't do it it will return 0, which means we gotta check on it...
      if(errorCode == 0)
      {
        // Get the error code
        DWORD err = GetLastError();
        std::cout << "[kachemak/Windows API] "
                  << "Couldn't create a symbolic link to " << m_szFolderName.string() << "!"
                  << " Windows error message code: " << err << std::endl;
      }
      else
      {
        // If you are at this point, then congratulations, you have survived the curses of Windows API, get yourself a
        // cookie.
        std::cout << "[kachemak]"
                  << "Created a symlink to: " << m_szFolderName.string() << "!" << std::endl;
      }
    }
  }
  return 0;
#else
  return 0;  // leave this for now
  for(const auto& item : TO_SYMLINK)
  {
    const std::filesystem::path symlinkPath = m_szSourcemodPath / m_szFolderName / item[1];
    if(!std::filesystem::exists(symlinkPath))
    {
      std::filesystem::create_symlink(m_szSourcemodPath / item[0], symlinkPath);
    }
  }

  return 0;
#endif
}

int Kachemak::verify()
{
  std::optional<KachemakVersion> installedKMVersion = get_km_version(m_szInstalledVersion);
  if(!installedKMVersion)
    return 4;
  // full signature url
  std::stringstream sigUrlFull_ss;
  sigUrlFull_ss << m_szSourceUrl << installedKMVersion.value().szSignature;
  // Data path for current install
  std::filesystem::path dataDir_path = m_szSourcemodPath / m_szFolderName;
  std::stringstream healUrl_ss;
  healUrl_ss << m_szSourceUrl << installedKMVersion.value().szFileName;
  int verifyRes = butler_verify(sigUrlFull_ss.str(), dataDir_path.string(), healUrl_ss.str());
  force_verify = false;
  write_version();
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

int Kachemak::update()
{
  A_printf("[Kachemak/update] Updating %s... ", m_szFolderName.c_str());
  if(m_szInstalledVersion == get_latest_version() || force_verify)
  {
    return 3;
  }
  int symlinkRes = prepare_symlink();
  if(symlinkRes != 0)
  {
    return 1;
  }

  std::optional<KachemakPatch> patch = get_patch(m_szInstalledVersion);
  if(!patch)
    return 3;

  if(free_space_check(patch.value().lTempRequired, FreeSpaceCheckCategory::Permanent) != 0)
  {
    return 2;
  }

  std::optional<KachemakVersion> installedVersion = get_km_version(m_szInstalledVersion);
  if(!installedVersion)
    return 4;

  // full signature url
  std::stringstream sigUrlFull_ss;
  sigUrlFull_ss << m_szSourceUrl << installedVersion.value().szSignature;
  // Data path for current install
  std::filesystem::path dataDir_path = m_szSourcemodPath / m_szFolderName;
  std::stringstream healUrl_ss;
  healUrl_ss << m_szSourceUrl << installedVersion.value().szFileName;
  int verifyRes = butler_verify(sigUrlFull_ss.str(), dataDir_path.string(), healUrl_ss.str());

  std::stringstream patchUrlFull_ss;
  patchUrlFull_ss << m_szSourceUrl << patch.value().szUrl;
  std::filesystem::path stagingPath = m_szSourcemodPath / ("butler-staging-" + m_szFolderName.string());
  A_printf("[Kachemak/update] Patching %s from %s to %s, with staging dir at %s. ", m_szFolderName.c_str(),
           installedVersion.value().szVersion.c_str(), get_latest_version().c_str(), stagingPath.c_str());
  int patchRes = butler_patch(patchUrlFull_ss.str(), stagingPath.string(), patch.value().szFilename,
                             dataDir_path.string(), patch.value().lTempRequired);

  do_symlink();
  m_szInstalledVersion = get_latest_version();
  write_version();
  return 0;
}

int Kachemak::install()
{
  if(prepare_symlink() != 0)
  {
    return 1;
  }
  std::optional<KachemakVersion> latestVersion = get_latest_km_version();
  if(!latestVersion)
    return 2;
  int diskSpaceStatus = free_space_check(latestVersion.value().lDownloadSize, FreeSpaceCheckCategory::Temporary);
  if(diskSpaceStatus != 0)
    return diskSpaceStatus;
  std::string downloadUri = m_szSourceUrl + latestVersion.value().szDownloadUrl;
  A_printf("[Kachemak/install] Downloading via torrent...");
  int downloadStatus = torrent::libtorrent_download(downloadUri, m_szTempPath.string(), &m_eventSystem);
  // std::filesystem::path path = net::download_to_temp(downloadUri, latestVersion.value().szFileName,
  // true,&m_eventSystem);
  if(downloadStatus != 0)
  {
    A_error("[Kachemak/install] Download failed - ret val %d \n", downloadStatus);
    return downloadStatus;
  }
  A_printf("[Kachemak/install] Download complete: extracting...");
  std::filesystem::create_directory(m_szSourcemodPath.string() / m_szFolderName);
  int err_c = extract(latestVersion.value().szFileName, (m_szSourcemodPath / m_szFolderName).string(),
                      latestVersion.value().lExtractSize);
  // extract( path.string() , (m_szSourcemodPath/ m_szFolderName).string() , latestVersion.value().lExtractSize);
  if(err_c == 0)
  {
    A_printf("[Kachemak/install] Extraction done!");
    do_symlink();
    m_szInstalledVersion = get_latest_version();
    write_version();
    return 0;
  }
  else
  {
    return 1;
  }
}

int Kachemak::install_path(std::filesystem::path customPath)
{
  if(prepare_symlink() != 0)
  {
    return 1;
  }
  std::optional<KachemakVersion> latestVersion = get_latest_km_version();
  if(!latestVersion)
    return 2;
  int diskSpaceStatus = free_space_check(latestVersion.value().lDownloadSize, FreeSpaceCheckCategory::Temporary);
  if(diskSpaceStatus != 0)
    return diskSpaceStatus;
  std::string downloadUri = m_szSourceUrl + latestVersion.value().szDownloadUrl;
  A_printf("[Kachemak/InstallInPath] Downloading via torrent...");
  int downloadStatus = torrent::libtorrent_download(downloadUri, m_szTempPath.string(), &m_eventSystem);
  // std::filesystem::path path = net::download_to_temp(downloadUri, latestVersion.value().szFileName,
  // true,&m_eventSystem);
  if(downloadStatus != 0)
  {
    A_error("[Kachemak/InstallInPath] Download failed - ret val %d \n", downloadStatus);
    return downloadStatus;
  }
  A_printf("[Kachemak/InstallInPath] Download complete: extracting...");
  std::filesystem::create_directory(customPath.string() / m_szFolderName);
  int err_c = extract_path(latestVersion.value().szFileName, (customPath / m_szFolderName).string(),
                             latestVersion.value().lExtractSize);
  // extract( path.string() , (m_szSourcemodPath/ m_szFolderName).string() , latestVersion.value().lExtractSize);
  if(err_c == 0)
  {
    A_printf("[Kachemak/InstallInPath] Extraction done!");
    do_symlink();
    m_szInstalledVersion = get_latest_version();
    write_version();
    return 0;
  }
  else
  {
    return 1;
  }
}

/*
desc:
        extract .zip file to directory
res:
        0: success
        1: not enough free space
*/
int Kachemak::extract(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize)
{
  A_printf("[Kachemak/extract] Input File %s, Output %s, size %lu", szInputFile.c_str(), szOutputDirectory.c_str(),
           szSize);
  if(free_space_check(szSize * 2, FreeSpaceCheckCategory::Permanent) != 0)
  {
    A_printf("[Kachemak/extract] Not enough space. Exiting.");
    return 1;
  }
  int ret = sys::extract_zip((m_szTempPath / szInputFile).string(), szOutputDirectory);
  if(ret != 0)
  {
    A_error("[Kachemak/extract] Extraction Failed - %s\n", zip_strerror(ret));
    return -1;
  }
  m_szInstalledVersion = get_latest_version();
  write_version();
  return 0;
}

int Kachemak::extract_path(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize)
{
  A_printf("[Kachemak/ExtractInPath] Input File %s, Output %s, size %lu", szInputFile.c_str(),
           szOutputDirectory.c_str(), szSize);

  // Variable to get the path
  std::filesystem::path sanitizedPath = std::filesystem::u8path(szOutputDirectory);
  if(free_space_check_path(szSize * 2, sanitizedPath) != 0)
  {
    A_printf("[Kachemak/ExtractInPath] Not enough space. Exiting.");
    return 1;
  }
  int ret = sys::extract_zip((m_szTempPath / szInputFile).string(), szOutputDirectory);
  if(ret != 0)
  {
    A_error("[Kachemak/ExtractInPath] Extraction Failed - %s\n", zip_strerror(ret));
    return -1;
  }
  m_szInstalledVersion = get_latest_version();
  write_version();
  return 0;
}

/*
description:
  wrapper for butler cli to verify installation
res:
  0: success
  i: return code from butler (when not 0)
*/
int Kachemak::butler_verify(const std::string& szSignature, const std::string& szGameDir, const std::string& szRemote)
{
  std::stringstream params;

  // {m_butlerLoation} verify {szSignature} {szGameDir} --heal=archive{szRemote}
  params << m_szButlerLocation.string() << " verify " << szSignature.c_str() << " "
         << "\"" << szGameDir.c_str() << "\""
         << " "
         << "--heal=archive," << szRemote << " --json";
  A_printf(szSignature.c_str());
  int status = butler_parse_command(params.str());
  if(status != NULL)
    return status;

  return 0;
}
/*
description:
  patch installation
res:
  0: success
  1: failed to download patch
  2: failed to apply patch*/
int Kachemak::butler_patch(const std::string& sz_url, const std::filesystem::path& sz_stagingDir,
                          const std::string& sz_patchFileName, const std::string& sz_gameDir,
                          const uintmax_t downloadSize)
{
  bool stagingDir_exists = std::filesystem::exists(sz_stagingDir);
  bool stagingDir_isDir = std::filesystem::is_directory(sz_stagingDir);
  if(!stagingDir_exists)
  {
    if(!std::filesystem::create_directory(sz_stagingDir))
    {
      A_printf("[Kachemak/butler_patch] Failed to create directory: %s", sz_stagingDir.c_str());
    }
  }
  if(stagingDir_exists && stagingDir_isDir)
  {
    switch(sys::delete_directory_content(sz_stagingDir))
    {
      case 1:
        A_printf("[Kachemak/butler_patch] Failed to delete staging directory content "
                 "(doesn't exist)");
        break;
      case 2:
        A_printf("[Kachemak/butler_patch] Failed to delete staging directory content "
                 "(not a directory)");
        break;
    }
  }

  int diskSpaceStatus = free_space_check(downloadSize, FreeSpaceCheckCategory::Temporary);
  if(diskSpaceStatus != 0)
    return diskSpaceStatus;

  // temporarily setting up to download through libtorrent instead

  A_printf("[Kachemak/butler_patch] Downloading through butler_patch()");
  int downloadStatus = torrent::libtorrent_download(sz_url, m_szTempPath.string());
  // net::download_to_temp(sz_url, sz_patchFileName, true,&m_eventSystem,&m_szTempPath);
  if(downloadStatus != 0)
    return downloadStatus;

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

  A_printf("[Kachemak/butler_patch] Applying patch.");
  if(int butlerStatus = butler_parse_command(params.str()); butlerStatus != NULL)
  {
    A_printf("[Kachemak/butler_patch] Failed to apply patch: %s", butlerStatus);
    return 2;
  }

  switch(sys::delete_directory_content(sz_stagingDir))
  {
    case 1:
      A_printf("[Kachemak/butler_patch] Failed to delete staging directory content "
               "(doesn't exist)");
      break;
    case 2:
      A_printf("[Kachemak/butler_patch] Failed to delete staging directory content "
               "(not a directory)");
      break;
  }
  return 0;
}

int Kachemak::butler_parse_command(const std::string& command)
{
  FILE* pipe = popen(command.c_str(), "r");
  if(!pipe)
  {
    A_printf("Failed to create pipe for butler verification");
    return 1;
  }

  const uint16_t bufSize = 4096;
  char buffer[bufSize];
  while(fgets(buffer, bufSize, pipe))
  {
    nlohmann::json jsonBuffer = nlohmann::json::parse(buffer, nullptr, false);
    if(jsonBuffer.is_discarded())
      continue;

    if(jsonBuffer.contains("type"))
    {
      std::string messageType = jsonBuffer["type"].get<std::string>();

      if(messageType.compare("progress") == NULL)
      {
        ProgressUpdateMessage message(jsonBuffer["bps"].get<float>(), jsonBuffer["progress"].get<float>());
        m_eventSystem.TriggerEvent(message);
      }
      else if(messageType.compare("error") == NULL)
      {
        ErrorMessage message(jsonBuffer["message"].get<std::string>());
        m_eventSystem.TriggerEvent(message);
      }
    }
  }

  pclose(pipe);

  return 0;
}
void Kachemak::find_installed_version()
{
  if(exists(m_szSourcemodPath / m_szFolderName))
  {
    std::ifstream data(m_szSourcemodPath / m_szFolderName / ".adastral");
    if(!data.fail())
    {
      nlohmann::json filedata = nlohmann::json::parse(data);
      m_szInstalledVersion = filedata["version"];
      A_printf("[Kachemak/InstalledVersion] %s version: %s", m_szFolderName.c_str(), get_installed_version_tag().c_str());
    }
    else
    {
      A_printf("[Kachemak/InstalledVersion] Adastral supported game detected (%s), but .adastral not detected.\n"
               "Assuming best case and setting force_version.",
               m_szFolderName.c_str());
      m_szInstalledVersion = get_latest_version();
      force_verify = true;
    }
  }
  else
  {
    A_printf("[Kachemak/InstalledVersion] %s not installed.", m_szFolderName.c_str());
  }
}

std::string Kachemak::get_installed_version_tag()
{
  std::string versionId = get_installed_version();
  if(m_parsedVersion["versions"][versionId].contains("tag"))
  {
    return m_parsedVersion["versions"][versionId]["tag"].get<std::string>();
  };
  return versionId;
}

std::string Kachemak::get_latest_version()
{
  std::string versionId;
  return m_parsedVersion["latest"];
}

std::string Kachemak::get_latest_version_tag()
{
  std::string versionId = get_latest_version();
  if(m_parsedVersion["versions"][versionId].contains("tag"))
  {
    return m_parsedVersion["versions"][versionId]["tag"].get<std::string>();
  };
  return versionId;
}
void Kachemak::write_version()
{
  nlohmann::json test_json;
  test_json["version"] = m_szInstalledVersion;
  std::ofstream data(m_szSourcemodPath / m_szFolderName / ".adastral");
  data << test_json;
  data.close();
}
