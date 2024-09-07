#include <kachemak/kachemak.hpp>

Kachemak::Kachemak(const std::filesystem::path &sourcemod_path, const std::filesystem::path &folder_name, const std::string &source_url, const std::filesystem::path &butler_path)
    : Version(sourcemod_path, folder_name, source_url)
{
    // placeholder
    temp_path = std::filesystem::temp_directory_path().string();
    name = folder_name.string(); // this is bad don't do this
    butler_location = butler_path;
    std::string ver_string = net().get_string_data_from_server(source_url + "bullseye.json");
    if (!nlohmann::json::accept(ver_string))
    {
        throw std::runtime_error("INVALID JSON. \n " + source_url + "bullseye.json");
    }
    parsed_version = nlohmann::ordered_json::parse(ver_string);
    find_installed_version();
    event_system.register_listener(EventType::OnUpdate,
                                     [](Event &ev)
                                     {
                                         // long double prog = ((ProgressUpdateMessage&)ev).get_progress();
                                         // long double prog2 = ((ProgressUpdateMessage&)ev).get_progress();
                                         // A_printf("[Kachemak/Butler] Progress: %d (unrounded: %f)\n",
                                         // round(prog2*100),prog);
                                     });
}

std::optional<KachemakVersion> Kachemak::get_km_version(const std::string &version)
{
    nlohmann::ordered_json &jsonVersion = parsed_version["versions"][version];
    if (!jsonVersion.is_object())
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

std::optional<KachemakPatch> Kachemak::get_patch(const std::string &version)
{ // this doesn't work if there's missing fields
    std::string latest = parsed_version["latest"];
    nlohmann::ordered_json &json_patches = parsed_version["patches"][latest][version];
    if (!json_patches.is_object())
    {
        A_printf("[Kachemak/get_patch] Failed to find patch %s", version.c_str());
        return std::nullopt;
    }

    KachemakPatch ret = {
        .url = json_patches["file_p2p"].get<std::string>(),
        .filename = json_patches["file"].get<std::string>(),
        .temp_required = json_patches["tempreq"].get<std::uintmax_t>(),
    };

    return ret;
}

std::optional<KachemakVersion> Kachemak::get_latest_km_version()
{
    return get_km_version(parsed_version["latest"]);
}

/*
description:
  check free space for specific category provided.
res:
  0: success
  1: not enough temp storage
  2: not enough permanent storage
*/
int Kachemak::free_space_check(const uintmax_t size, const FreeSpaceCheckCategory &category)
{
    switch (category)
    {
    case FreeSpaceCheckCategory::Temporary:
        if (std::filesystem::space(temp_path).free < size)
        {
            return 1;
        }
        break;
    case FreeSpaceCheckCategory::Permanent:
        if (std::filesystem::space(sourcemod_path).free < size)
        {
            return 2;
        }
        break;
    }
    return 0;
}

int Kachemak::free_space_check_path(const uintmax_t size, const std::filesystem::path custom_path)
{
    if (std::filesystem::space(custom_path).free < size)
    {
        return 1;
    }
    return 0;
}


int Kachemak::CreateSymlink(std::filesystem::path customPath) {
#ifdef _WIN32
  // Abandon all hope, ye who enter there, for I am pulling the ancient arts of Windows API.
  // So ancient, even if the IDE complains about the strings it actually works just fine.

 
    // First, get the path and check if it would even exist
    const std::filesystem::path symlinkPath = sourcemod_path / folder_name;
    std::cout << customPath << std::endl << symlinkPath << std::endl;

    // Then check if they even or not. If they don't exist in the path itself, then one can create them.
    if (!std::filesystem::exists(symlinkPath)) {
      // Do the symbolic link through CreateSymbolicLink function.
      int errorCode = CreateSymbolicLinkW(symlinkPath.wstring().c_str(), customPath.wstring().c_str(),
                                          SYMBOLIC_LINK_FLAG_DIRECTORY);

      // Hopefully it should work just fine, but if it can't do it it will return 0, which means we gotta check on it...
      if (errorCode == 0) {
        // Get the error code
        DWORD err = GetLastError();
        std::cout << "[kachemak/CreateSymlink/Windows API] "
                  << "Couldn't create a symbolic link to " << folder_name.string() << "!"
                  << " Windows error message code: " << err << std::endl;

        //If the error code  is ERROR_PRIVILEGE_NOT_HELD, then it needs admin privileges, so we ask gently for them.

        
        if (err == ERROR_PRIVILEGE_NOT_HELD) {
          std::cout << "[kachemak/CreateSymlink/Windows API]"
                    << "Attempting to run with elevated privileges..." << std::endl;
            return 0;
          //wchar_t exePath[MAX_PATH];
          //GetModuleFileNameW(NULL, exePath, MAX_PATH);

          //SHELLEXECUTEINFOW sei = {sizeof(sei)};
          //sei.fMask = SEE_MASK_NOCLOSEPROCESS;
          //sei.hwnd = NULL;
          //sei.lpVerb = L"runas";    // Request elevation
          //sei.lpFile = exePath;     // The current executable
          //sei.lpParameters = NULL;  
          //sei.lpDirectory = NULL;
          //sei.nShow = SW_SHOWNORMAL;

          //if (ShellExecuteExW(&sei)) {
          //  // Close the current (non-elevated) process
          //  ExitProcess(0);  // Exit immediately
          //} else {
          //  std::cout << "[kachemak/CreateSymlink/Windows API]"
          //            << "Failed to run in elevated privileges. Error: " << GetLastError() << std::endl;
          //
          //}
        }

      } else {
        // If you are at this point, then congratulations, you have survived the curses of Windows API, get yourself a
        // cookie.
        std::cout << "[kachemak/CreateSymlink/Windows API]"
                  << "Created a symlink to: " << folder_name.string() << "!" << std::endl;
      }
    }
    #else
    //Add in for linux
#endif
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
    A_printf("[Kachemak/update] Updating %s... ", folder_name.c_str());
    if (installed_version_code == get_latest_version_code() || force_verify)
    {
        return 3;
    }

    std::optional<KachemakPatch> patch = get_patch(installed_version_code);
    if (!patch)
    {
        return 3;
    }

    if (free_space_check(patch.value().temp_required, FreeSpaceCheckCategory::Permanent) != 0)
    {
        return 2;
    }

    std::optional<KachemakVersion> km_installed_version = get_km_version(installed_version_code);
    if (!km_installed_version)
    {
        return 4;
    }

    // full signature url
    std::stringstream sig_url_full;
    sig_url_full << source_url << km_installed_version.value().signature;
    // Data path for current install
    std::filesystem::path data_dir_path = sourcemod_path / folder_name;
    std::stringstream heal_url;
    heal_url << source_url << km_installed_version.value().file_name;
    int verifyRes = butler_verify(sig_url_full.str(), data_dir_path.string(), heal_url.str());
    std::stringstream patch_url_full;
    patch_url_full << source_url << patch.value().url;
    std::filesystem::path staging_path = sourcemod_path / ("butler-staging-" + folder_name.string());
    A_printf("[Kachemak/update] Patching %s from %s to %s, with staging dir at %s. ", folder_name.c_str(), km_installed_version.value().version.c_str(), get_latest_version_code().c_str(),
             staging_path.c_str());
    int patchRes = butler_patch(patch_url_full.str(), staging_path.string(), patch.value().filename, data_dir_path.string(), patch.value().temp_required);

    installed_version_code = get_latest_version_code();
    write_version();
    return 0;
}

int Kachemak::Update_InPath() {
  //A_printf("[Kachemak/Update_I] Updating %s... ", folder_name.c_str());
  //if (installed_version_code == GetLatestVersion() || force_verify) {
  //  return 3;
  //}
  //int symlinkRes = PrepareSymlink();
  //if (symlinkRes != 0) {
  //  return 1;
  //}

  //std::optional<KachemakPatch> patch = GetPatch(m_szInstalledVersion);
  //if (!patch) return 3;

  //if (FreeSpaceCheck(patch.value().lTempRequired, FreeSpaceCheckCategory::Permanent) != 0) {
  //  return 2;
  //}

  //std::optional<KachemakVersion> installedVersion = GetKMVersion(m_szInstalledVersion);
  //if (!installedVersion) return 4;

  //// full signature url
  //std::stringstream sigUrlFull_ss;
  //sigUrlFull_ss << source_url << installedVersion.value().szSignature;
  //// Data path for current install
  //std::filesystem::path dataDir_path = m_szSourcemodPath / folder_name;
  //std::stringstream healUrl_ss;
  //healUrl_ss << source_url << installedVersion.value().szFileName;
  //int verifyRes = butler_verify(sigUrlFull_ss.str(), dataDir_path.string(), healUrl_ss.str());

  //std::stringstream patchUrlFull_ss;
  //patchUrlFull_ss << source_url << patch.value().szUrl;
  //std::filesystem::path stagingPath = m_szSourcemodPath / ("butler-staging-" + folder_name.string());
  //A_printf("[Kachemak/Update] Patching %s from %s to %s, with staging dir at %s. ", folder_name.c_str(),
  //         installedVersion.value().szVersion.c_str(), GetLatestVersion().c_str(), stagingPath.c_str());
  //int patchRes = ButlerPatch(patchUrlFull_ss.str(), stagingPath.string(), patch.value().szFilename,
  //                           dataDir_path.string(), patch.value().lTempRequired);

  //DoSymlink();
  //m_szInstalledVersion = GetLatestVersion();
  //WriteVersion();
  return 0;
}
int Kachemak::install()
{
    std::optional<KachemakVersion> latest_version = get_latest_km_version();
    if (!latest_version)
    {
        return 2;
    }
    int disk_space_status = free_space_check(latest_version.value().download_size, FreeSpaceCheckCategory::Temporary);
    if (disk_space_status != 0)
    {
        return disk_space_status;
    }
    std::string download_uri = source_url + latest_version.value().download_url;
    A_printf("[Kachemak/install] Downloading via torrent...");
    int download_status = torrent::libtorrent_download(download_uri, temp_path.string(), &event_system);
    if (download_status != 0)
    {
        A_error("[Kachemak/install] Download failed - ret val %d \n", download_status);
        return download_status;
    }
    A_printf("[Kachemak/install] Download complete: extracting...");
    std::filesystem::create_directory(sourcemod_path.string() / folder_name);
    int ret = extract(latest_version.value().file_name, (sourcemod_path / folder_name).string(), latest_version.value().extract_size);
    if (ret == 0)
    {
        A_printf("[Kachemak/install] Extraction done!");
        installed_version_code = get_latest_version_code();
        write_version();
        return 0;
    }
    else
    {
        return 1;
    }
}

int Kachemak::verify()
{
    std::optional<KachemakVersion> installed_km_version = get_km_version(installed_version_code);
    if (!installed_km_version)
    {
        return 4;
    }
    // full signature url
    std::stringstream sig_url_full;
    sig_url_full << source_url << installed_km_version.value().signature;
    // Data path for current install
    std::filesystem::path dataDir_path = sourcemod_path / folder_name;
    std::stringstream heal_url;
    heal_url << source_url << installed_km_version.value().file_name;
    butler_verify(sig_url_full.str(), dataDir_path.string(), heal_url.str());
    force_verify = false;
    write_version();
    return 0;
}

int Kachemak::install_path(std::filesystem::path custom_path)
{
    std::optional<KachemakVersion> latest_version = get_latest_km_version();
    if (!latest_version)
    {
        return 2;
    }
    int disk_space_status = free_space_check_path(latest_version.value().download_size, custom_path);
    if (disk_space_status != 0)
    {
        return disk_space_status;
    }
    std::string download_uri = source_url + latest_version.value().download_url;
    A_printf("[Kachemak/InstallInPath] Downloading via torrent...");
    int download_status = torrent::libtorrent_download(download_uri, temp_path.string(), &event_system);
    // std::filesystem::path path = net::download_to_temp(downloadUri, latestVersion.value().file_name,
    // true,&event_system);
    if (download_status != 0)
    {
        A_error("[Kachemak/InstallInPath] Download failed - ret val %d \n", download_status);
        return download_status;
    }
    A_printf("[Kachemak/InstallInPath] Download complete: extracting...");
    std::filesystem::create_directory(custom_path.string() / folder_name);
    int ret = extract_path(latest_version.value().file_name, (custom_path / folder_name).string(), latest_version.value().extract_size);
    // extract( path.string() , (sourcemod_path/ folder_name).string() , latestVersion.value().extract_size);
    if (ret == 0)
    {
        A_printf("[Kachemak/InstallInPath] Extraction done!");
        installed_version_code = get_latest_version_code();
        write_version();
        return 0;
    }
    return 1;
}

/*
desc:
        extract .zip file to directory
res:
        0: success
        1: not enough free space
*/
int Kachemak::extract(const std::string &input_file, const std::string &output_directory, const size_t &size)
{
    A_printf("[Kachemak/extract] Input File %s, Output %s, size %lu", input_file.c_str(), output_directory.c_str(), size);
    if (free_space_check(size * 2, FreeSpaceCheckCategory::Permanent) != 0)
    {
        A_printf("[Kachemak/extract] Not enough space. Exiting.");
        return 1;
    }
    int ret = sys::extract_zip((temp_path / input_file).string(), output_directory);
    if (ret != 0)
    {
        A_error("[Kachemak/extract] Extraction Failed - %s\n", zip_strerror(ret));
        return -1;
    }
    installed_version_code = get_latest_version_code();
    write_version();
    return 0;
}

int Kachemak::extract_path(const std::string &input_file, const std::string &output_directory, const size_t &size)
{
    A_printf("[Kachemak/ExtractInPath] Input File %s, Output %s, size %lu", input_file.c_str(), output_directory.c_str(), size);

    // Variable to get the path
    std::filesystem::path sanitized_path = std::filesystem::u8path(output_directory);
    if (free_space_check_path(size * 2, sanitized_path) != 0)
    {
        A_printf("[Kachemak/ExtractInPath] Not enough space. Exiting.");
        return 1;
    }
    int ret = sys::extract_zip((temp_path / input_file).string(), output_directory);
    if (ret != 0)
    {
        A_error("[Kachemak/ExtractInPath] Extraction Failed - %s\n", zip_strerror(ret));
        return -1;
    }
    installed_version_code = get_latest_version_code();
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
int Kachemak::butler_verify(const std::string &signature, const std::string &game_dir, const std::string &remote)
{
    std::stringstream params;

    // {m_butlerLoation} verify {signature} {game_dir} --heal=archive{remote}
    params << butler_location.string() << " verify " << signature.c_str() << " "
           << "\"" << game_dir.c_str() << "\""
           << " "
           << "--heal=archive," << remote << " --json";
    A_printf(signature.c_str());
    int status = butler_parse_command(params.str());
    if (status != NULL)
    {
        return status;
    }

    return 0;
}
/*
description:
  patch installation
res:
  0: success
  1: failed to download patch
  2: failed to apply patch*/
int Kachemak::butler_patch(const std::string &url, const std::filesystem::path &staging_dir, const std::string &patch_file_name, const std::string &game_dir, const uintmax_t download_size)
{
    bool staging_dir_exists = std::filesystem::exists(staging_dir);
    bool staging_dir_is_dir = std::filesystem::is_directory(staging_dir);
    if (!staging_dir_exists)
    {
        if (!std::filesystem::create_directory(staging_dir))
        {
            A_printf("[Kachemak/butler_patch] Failed to create directory: %s", staging_dir.c_str());
        }
    }
    if (staging_dir_exists && staging_dir_is_dir)
    {
        switch (sys::delete_directory_content(staging_dir))
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

    int disk_space_status = free_space_check(download_size, FreeSpaceCheckCategory::Temporary);
    if (disk_space_status != 0)
    {
        return disk_space_status;
    }

    // temporarily setting up to download through libtorrent instead

    A_printf("[Kachemak/butler_patch] Downloading through butler_patch()");
    int download_status = torrent::libtorrent_download(url, temp_path.string());
    if (download_status != 0)
    {
        return download_status;
    }

    std::filesystem::path temp_path = temp_path / patch_file_name;

    std::stringstream params;

    params << butler_location.string() << " ";
    params << "apply"
           << " ";
    params << "--staging-dir=\"" << staging_dir.string() << "\""
           << " ";
    params << "\"" << temp_path.string() << "\""
           << " ";
    params << "\"" << game_dir << "\""
           << " ";
    params << "--json";

    A_printf("[Kachemak/butler_patch] Applying patch.");
    if (int butler_status = butler_parse_command(params.str()); butler_status != NULL)
    {
        A_printf("[Kachemak/butler_patch] Failed to apply patch: %s", butler_status);
        return 2;
    }

    switch (sys::delete_directory_content(staging_dir))
    {
    case 1:
        A_printf("[Kachemak/butler_patch] Failed to delete staging directory content (doesn't exist)");
        break;
    case 2:
        A_printf("[Kachemak/butler_patch] Failed to delete staging directory content (not a directory)");
        break;
    }
    return 0;
}

int Kachemak::butler_parse_command(const std::string &command)
{
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        A_printf("Failed to create pipe for butler verification");
        return 1;
    }

    const uint16_t bufSize = 4096;
    char buffer[bufSize];
    while (fgets(buffer, bufSize, pipe))
    {
        nlohmann::json json_buffer = nlohmann::json::parse(buffer, nullptr, false);
        if (json_buffer.is_discarded())
        {
            continue;
        }

        if (json_buffer.contains("type"))
        {
            std::string message_type = json_buffer["type"].get<std::string>();
            if (message_type.compare("progress") == NULL)
            {
                ProgressUpdateMessage message(json_buffer["bps"].get<float>(), json_buffer["progress"].get<float>());
                event_system.trigger_event(message);
            }
            else if (message_type.compare("error") == NULL)
            {
                ErrorMessage message(json_buffer["message"].get<std::string>());
                event_system.trigger_event(message);
            }
        }
    }

    pclose(pipe);

    return 0;
}
void Kachemak::find_installed_version()
{
    if (exists(sourcemod_path / folder_name))
    {
        std::ifstream data(sourcemod_path / folder_name / ".adastral");
        if (!data.fail())
        {
            nlohmann::json file_data = nlohmann::json::parse(data);
            installed_version_code = file_data["version"];
            A_printf("[Kachemak/InstalledVersion] %s version: %s", folder_name.c_str(), get_installed_version_tag().c_str());
        }
        else
        {
            A_printf("[Kachemak/InstalledVersion] Adastral supported game detected (%s), but .adastral not detected.\n"
                     "Assuming best case and setting force_version.",
                     folder_name.c_str());
            installed_version_code = get_latest_version_code();
            force_verify = true;
        }
    }
    else
    {
        A_printf("[Kachemak/InstalledVersion] %s not installed.", folder_name.c_str());
    }
}

std::string Kachemak::get_installed_version_tag()
{
    std::string version_id = get_installed_version_code();
    if (parsed_version["versions"][version_id].contains("tag"))
    {
        return parsed_version["versions"][version_id]["tag"].get<std::string>();
    };
    return version_id;
}

std::string Kachemak::get_latest_version_code()
{
    return parsed_version["latest"];
}

std::string Kachemak::get_latest_version_tag()
{
    std::string version_id = get_latest_version_code();
    if (parsed_version["versions"][version_id].contains("tag"))
    {
        return parsed_version["versions"][version_id]["tag"].get<std::string>();
    };
    return version_id;
}
void Kachemak::write_version()
{
    nlohmann::json test_json;
    test_json["version"] = installed_version_code;
    std::ofstream data(sourcemod_path / folder_name / ".adastral");
    data << test_json;
    data.close();
}
