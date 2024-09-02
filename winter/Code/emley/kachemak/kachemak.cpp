#include <kachemak/kachemak.hpp>

Kachemak::Kachemak(const std::filesystem::path& sourcemod_path, const std::filesystem::path& folder_name,
                   const std::string& source_url, const std::filesystem::path& butler_path) :
    Version(sourcemod_path, folder_name, source_url)
{
  // placeholder
  M_temp_path = std::filesystem::temp_directory_path().string();
  name = folder_name.string();  // this is bad don't do this
  M_butler_location = butler_path;
  std::string ver_string = net().get_string_data_from_server(source_url + "bullseye.json");
  if(!nlohmann::json::accept(ver_string))
  {
    throw std::runtime_error("INVALID JSON. \n " + source_url + "bullseye.json");
  }
  M_parsed_version = nlohmann::ordered_json::parse(ver_string);
  find_installed_version();
  M_event_system.register_listener(EventType::kOnUpdate,
                                 [](Event& ev)
                                 {
                                   // long double prog = ((ProgressUpdateMessage&)ev).get_progress();
                                   // long double prog2 = ((ProgressUpdateMessage&)ev).get_progress();
                                   // A_printf("[Kachemak/Butler] Progress: %d (unrounded: %f)\n",
                                   // round(prog2*100),prog);
                                 });
}

std::optional<KachemakVersion> Kachemak::get_km_version(const std::string& version)
{
  nlohmann::ordered_json& jsonVersion = M_parsed_version["versions"][version];
  if(!jsonVersion.is_object())
  {
    A_printf("[Kachemak/get_km_version] Failed to find version %s", version.c_str());
    return std::nullopt;
  }

  KachemakVersion ret = {
    .file_name = jsonVersion["file"].get<std::string>(),
    //      .download_url = jsonVersion["url"].get<std::string>(),
    .download_url = jsonVersion["file_p2p"].get<std::string>(),
    //      .download_size = jsonVersion["presz"].get<std::size_t>(),
    //      .extract_size = jsonVersion["postsz"].get<std::size_t>(),
    .version = version,
    .signature = jsonVersion["sig"].get<std::string>(),
  };

  return ret;
}

std::optional<KachemakPatch> Kachemak::get_patch(const std::string& version)
{  // this doesn't work if there's missing fields
  std::string latest = M_parsed_version["latest"];
  nlohmann::ordered_json& jsonPatches = M_parsed_version["patches"][latest][version];
  if(!jsonPatches.is_object())
  {
    A_printf("[Kachemak/get_patch] Failed to find patch %s", version.c_str());
    return std::nullopt;
  }

  KachemakPatch ret = {
    .url = jsonPatches["file_p2p"].get<std::string>(),
    .filename = jsonPatches["file"].get<std::string>(),
    .temp_required = jsonPatches["tempreq"].get<std::uintmax_t>(),
  };

  return ret;
}

std::optional<KachemakVersion> Kachemak::get_latest_km_version() { return get_km_version(M_parsed_version["latest"]); }

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
      if(std::filesystem::space(M_temp_path).free < size)
      {
        return 1;
      }
      break;
    case FreeSpaceCheckCategory::Permanent:
      if(std::filesystem::space(M_sourcemod_path).free < size)
      {
        return 2;
      }
      break;
  }
  return 0;
}

int Kachemak::free_space_check_path(const uintmax_t size, const std::filesystem::path custom_path)
{
  if(std::filesystem::space(custom_path).free < size)
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
    const std::filesystem::path symlinkPath = M_sourcemod_path / item[1];
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
    const std::filesystem::path symlinkPath = M_sourcemod_path / M_folder_name / item[1];
    if(!std::filesystem::exists(symlinkPath))
    {
      std::filesystem::create_symlink(M_sourcemod_path / item[0], symlinkPath);
    }
  }

  return 0;
#endif
}

int Kachemak::do_symlink_path(std::filesystem::path custom_path)
{
#ifdef _WIN32
  // Abandon all hope, ye who enter there, for I am pulling the ancient arts of Windows API.
  // So ancient, even if the IDE complains about the strings it actually works just fine.

  for(const auto& item : TO_SYMLINK)
  {
    // First, get the path and check if it would even exist
    const std::filesystem::path symlinkPath = M_sourcemod_path / M_folder_name / item[1];

    // Then check if they even or not. If they don't exist in the path itself, then one can create them.
    if(!std::filesystem::exists(symlinkPath))
    {
      // Do the symbolic link through CreateSymbolicLink function.
      int errorCode = CreateSymbolicLinkW(symlinkPath.wstring().c_str(), custom_path.wstring().c_str(),
                                          SYMBOLIC_LINK_FLAG_DIRECTORY);

      // Hopefully it should work just fine, but if it can't do it it will return 0, which means we gotta check on it...
      if(errorCode == 0)
      {
        // Get the error code
        DWORD err = GetLastError();
        std::cout << "[kachemak/Windows API] "
                  << "Couldn't create a symbolic link to " << M_folder_name.string() << "!"
                  << " Windows error message code: " << err << std::endl;
      }
      else
      {
        // If you are at this point, then congratulations, you have survived the curses of Windows API, get yourself a
        // cookie.
        std::cout << "[kachemak]"
                  << "Created a symlink to: " << M_folder_name.string() << "!" << std::endl;
      }
    }
  }
  return 0;
#else
  return 0;  // leave this for now
  for(const auto& item : TO_SYMLINK)
  {
    const std::filesystem::path symlinkPath = M_sourcemod_path / M_folder_name / item[1];
    if(!std::filesystem::exists(symlinkPath))
    {
      std::filesystem::create_symlink(M_sourcemod_path / item[0], symlinkPath);
    }
  }

  return 0;
#endif
}

int Kachemak::verify()
{
  std::optional<KachemakVersion> installedKMVersion = get_km_version(M_installed_version);
  if(!installedKMVersion)
    return 4;
  // full signature url
  std::stringstream sigUrlFull_ss;
  sigUrlFull_ss << M_source_url << installedKMVersion.value().signature;
  // Data path for current install
  std::filesystem::path dataDir_path = M_sourcemod_path / M_folder_name;
  std::stringstream healUrl_ss;
  healUrl_ss << M_source_url << installedKMVersion.value().file_name;
  int verifyRes = butler_verify(sigUrlFull_ss.str(), dataDir_path.string(), healUrl_ss.str());
  M_force_verify = false;
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
  A_printf("[Kachemak/update] Updating %s... ", M_folder_name.c_str());
  if(M_installed_version == get_latest_version() || M_force_verify)
  {
    return 3;
  }
  int symlinkRes = prepare_symlink();
  if(symlinkRes != 0)
  {
    return 1;
  }

  std::optional<KachemakPatch> patch = get_patch(M_installed_version);
  if(!patch)
    return 3;

  if(free_space_check(patch.value().temp_required, FreeSpaceCheckCategory::Permanent) != 0)
  {
    return 2;
  }

  std::optional<KachemakVersion> installedVersion = get_km_version(M_installed_version);
  if(!installedVersion)
    return 4;

  // full signature url
  std::stringstream sigUrlFull_ss;
  sigUrlFull_ss << M_source_url << installedVersion.value().signature;
  // Data path for current install
  std::filesystem::path dataDir_path = M_sourcemod_path / M_folder_name;
  std::stringstream healUrl_ss;
  healUrl_ss << M_source_url << installedVersion.value().file_name;
  int verifyRes = butler_verify(sigUrlFull_ss.str(), dataDir_path.string(), healUrl_ss.str());

  std::stringstream patchUrlFull_ss;
  patchUrlFull_ss << M_source_url << patch.value().url;
  std::filesystem::path stagingPath = M_sourcemod_path / ("butler-staging-" + M_folder_name.string());
  A_printf("[Kachemak/update] Patching %s from %s to %s, with staging dir at %s. ", M_folder_name.c_str(),
           installedVersion.value().version.c_str(), get_latest_version().c_str(), stagingPath.c_str());
  int patchRes = butler_patch(patchUrlFull_ss.str(), stagingPath.string(), patch.value().filename,
                             dataDir_path.string(), patch.value().temp_required);

  do_symlink();
  M_installed_version = get_latest_version();
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
  int diskSpaceStatus = free_space_check(latestVersion.value().download_size, FreeSpaceCheckCategory::Temporary);
  if(diskSpaceStatus != 0)
    return diskSpaceStatus;
  std::string downloadUri = M_source_url + latestVersion.value().download_url;
  A_printf("[Kachemak/install] Downloading via torrent...");
  int downloadStatus = torrent::libtorrent_download(downloadUri, M_temp_path.string(), &M_event_system);
  // std::filesystem::path path = net::download_to_temp(downloadUri, latestVersion.value().file_name,
  // true,&M_event_system);
  if(downloadStatus != 0)
  {
    A_error("[Kachemak/install] Download failed - ret val %d \n", downloadStatus);
    return downloadStatus;
  }
  A_printf("[Kachemak/install] Download complete: extracting...");
  std::filesystem::create_directory(M_sourcemod_path.string() / M_folder_name);
  int err_c = extract(latestVersion.value().file_name, (M_sourcemod_path / M_folder_name).string(),
                      latestVersion.value().extract_size);
  // extract( path.string() , (M_sourcemod_path/ M_folder_name).string() , latestVersion.value().extract_size);
  if(err_c == 0)
  {
    A_printf("[Kachemak/install] Extraction done!");
    do_symlink();
    M_installed_version = get_latest_version();
    write_version();
    return 0;
  }
  else
  {
    return 1;
  }
}

int Kachemak::install_path(std::filesystem::path custom_path)
{
  if(prepare_symlink() != 0)
  {
    return 1;
  }
  std::optional<KachemakVersion> latestVersion = get_latest_km_version();
  if(!latestVersion)
    return 2;
  int diskSpaceStatus = free_space_check(latestVersion.value().download_size, FreeSpaceCheckCategory::Temporary);
  if(diskSpaceStatus != 0)
    return diskSpaceStatus;
  std::string downloadUri = M_source_url + latestVersion.value().download_url;
  A_printf("[Kachemak/InstallInPath] Downloading via torrent...");
  int downloadStatus = torrent::libtorrent_download(downloadUri, M_temp_path.string(), &M_event_system);
  // std::filesystem::path path = net::download_to_temp(downloadUri, latestVersion.value().file_name,
  // true,&M_event_system);
  if(downloadStatus != 0)
  {
    A_error("[Kachemak/InstallInPath] Download failed - ret val %d \n", downloadStatus);
    return downloadStatus;
  }
  A_printf("[Kachemak/InstallInPath] Download complete: extracting...");
  std::filesystem::create_directory(custom_path.string() / M_folder_name);
  int err_c = extract_path(latestVersion.value().file_name, (custom_path / M_folder_name).string(),
                             latestVersion.value().extract_size);
  // extract( path.string() , (M_sourcemod_path/ M_folder_name).string() , latestVersion.value().extract_size);
  if(err_c == 0)
  {
    A_printf("[Kachemak/InstallInPath] Extraction done!");
    do_symlink();
    M_installed_version = get_latest_version();
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
int Kachemak::extract(const std::string& input_file, const std::string& output_directory, const size_t& size)
{
  A_printf("[Kachemak/extract] Input File %s, Output %s, size %lu", input_file.c_str(), output_directory.c_str(),
           size);
  if(free_space_check(size * 2, FreeSpaceCheckCategory::Permanent) != 0)
  {
    A_printf("[Kachemak/extract] Not enough space. Exiting.");
    return 1;
  }
  int ret = sys::extract_zip((M_temp_path / input_file).string(), output_directory);
  if(ret != 0)
  {
    A_error("[Kachemak/extract] Extraction Failed - %s\n", zip_strerror(ret));
    return -1;
  }
  M_installed_version = get_latest_version();
  write_version();
  return 0;
}

int Kachemak::extract_path(const std::string& input_file, const std::string& output_directory, const size_t& size)
{
  A_printf("[Kachemak/ExtractInPath] Input File %s, Output %s, size %lu", input_file.c_str(),
           output_directory.c_str(), size);

  // Variable to get the path
  std::filesystem::path sanitizedPath = std::filesystem::u8path(output_directory);
  if(free_space_check_path(size * 2, sanitizedPath) != 0)
  {
    A_printf("[Kachemak/ExtractInPath] Not enough space. Exiting.");
    return 1;
  }
  int ret = sys::extract_zip((M_temp_path / input_file).string(), output_directory);
  if(ret != 0)
  {
    A_error("[Kachemak/ExtractInPath] Extraction Failed - %s\n", zip_strerror(ret));
    return -1;
  }
  M_installed_version = get_latest_version();
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
int Kachemak::butler_verify(const std::string& signature, const std::string& game_dir, const std::string& remote)
{
  std::stringstream params;

  // {m_butlerLoation} verify {signature} {game_dir} --heal=archive{remote}
  params << M_butler_location.string() << " verify " << signature.c_str() << " "
         << "\"" << game_dir.c_str() << "\""
         << " "
         << "--heal=archive," << remote << " --json";
  A_printf(signature.c_str());
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
int Kachemak::butler_patch(const std::string& url, const std::filesystem::path& staging_dir,
                          const std::string& patch_file_name, const std::string& game_dir,
                          const uintmax_t download_size)
{
  bool stagingDir_exists = std::filesystem::exists(staging_dir);
  bool stagingDir_isDir = std::filesystem::is_directory(staging_dir);
  if(!stagingDir_exists)
  {
    if(!std::filesystem::create_directory(staging_dir))
    {
      A_printf("[Kachemak/butler_patch] Failed to create directory: %s", staging_dir.c_str());
    }
  }
  if(stagingDir_exists && stagingDir_isDir)
  {
    switch(sys::delete_directory_content(staging_dir))
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

  int diskSpaceStatus = free_space_check(download_size, FreeSpaceCheckCategory::Temporary);
  if(diskSpaceStatus != 0)
    return diskSpaceStatus;

  // temporarily setting up to download through libtorrent instead

  A_printf("[Kachemak/butler_patch] Downloading through butler_patch()");
  int downloadStatus = torrent::libtorrent_download(url, M_temp_path.string());
  // net::download_to_temp(url, patch_file_name, true,&M_event_system,&M_temp_path);
  if(downloadStatus != 0)
    return downloadStatus;

  std::filesystem::path tempPath = M_temp_path / patch_file_name;

  std::stringstream params;

  params << M_butler_location.string() << " ";
  params << "apply"
         << " ";
  params << "--staging-dir=\"" << staging_dir.string() << "\""
         << " ";
  params << "\"" << tempPath.string() << "\""
         << " ";
  params << "\"" << game_dir << "\""
         << " ";
  params << "--json";

  A_printf("[Kachemak/butler_patch] Applying patch.");
  if(int butlerStatus = butler_parse_command(params.str()); butlerStatus != NULL)
  {
    A_printf("[Kachemak/butler_patch] Failed to apply patch: %s", butlerStatus);
    return 2;
  }

  switch(sys::delete_directory_content(staging_dir))
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
        M_event_system.trigger_event(message);
      }
      else if(messageType.compare("error") == NULL)
      {
        ErrorMessage message(jsonBuffer["message"].get<std::string>());
        M_event_system.trigger_event(message);
      }
    }
  }

  pclose(pipe);

  return 0;
}
void Kachemak::find_installed_version()
{
  if(exists(M_sourcemod_path / M_folder_name))
  {
    std::ifstream data(M_sourcemod_path / M_folder_name / ".adastral");
    if(!data.fail())
    {
      nlohmann::json filedata = nlohmann::json::parse(data);
      M_installed_version = filedata["version"];
      A_printf("[Kachemak/InstalledVersion] %s version: %s", M_folder_name.c_str(), get_installed_version_tag().c_str());
    }
    else
    {
      A_printf("[Kachemak/InstalledVersion] Adastral supported game detected (%s), but .adastral not detected.\n"
               "Assuming best case and setting force_version.",
               M_folder_name.c_str());
      M_installed_version = get_latest_version();
      M_force_verify = true;
    }
  }
  else
  {
    A_printf("[Kachemak/InstalledVersion] %s not installed.", M_folder_name.c_str());
  }
}

std::string Kachemak::get_installed_version_tag()
{
  std::string versionId = get_installed_version();
  if(M_parsed_version["versions"][versionId].contains("tag"))
  {
    return M_parsed_version["versions"][versionId]["tag"].get<std::string>();
  };
  return versionId;
}

std::string Kachemak::get_latest_version()
{
  std::string versionId;
  return M_parsed_version["latest"];
}

std::string Kachemak::get_latest_version_tag()
{
  std::string versionId = get_latest_version();
  if(M_parsed_version["versions"][versionId].contains("tag"))
  {
    return M_parsed_version["versions"][versionId]["tag"].get<std::string>();
  };
  return versionId;
}
void Kachemak::write_version()
{
  nlohmann::json test_json;
  test_json["version"] = M_installed_version;
  std::ofstream data(M_sourcemod_path / M_folder_name / ".adastral");
  data << test_json;
  data.close();
}
