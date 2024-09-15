#include <kachemak/kachemak.hpp>

Kachemak::Kachemak(const std::filesystem::path &game_path, const std::filesystem::path &folder_name, const std::string &source_url, const std::filesystem::path &butler_path)
    : Version(game_path, folder_name, source_url)
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

/**
description:
  check free space for specific category provided.
res:
  0: success
  1: not enough temp storage
  2: not enough permanent storage
*/
int Kachemak::free_space_check(const uintmax_t size, const FreeSpaceCheckCategory &category, std::string perm_path)
{
    if (perm_path.empty())
    {
        perm_path = game_path;
    }
    switch (category)
    {
    case FreeSpaceCheckCategory::Temporary:
        if (std::filesystem::space(temp_path).free < size)
        {
            return 1;
        }
        break;
    case FreeSpaceCheckCategory::Permanent:
        if (std::filesystem::space(perm_path).free < size)
        {
            return 2;
        }
        break;
    }
    return 0;
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
    std::filesystem::path data_dir_path = game_path / folder_name;
    std::stringstream heal_url;
    heal_url << source_url << installed_km_version.value().file_name;
    butler_verify(sig_url_full.str(), data_dir_path.string(), heal_url.str());
    force_verify = false;
    write_version();
    return 0;
}

/**
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
    std::stringstream heal_url;
    heal_url << source_url << km_installed_version.value().file_name;
    int verify_ret = butler_verify(sig_url_full.str(), game_path.string(), heal_url.str());
    std::stringstream patch_url_full;
    patch_url_full << source_url << patch.value().url;
    std::filesystem::path staging_path = game_path / ("butler-staging-" + folder_name.string());
    A_printf("[Kachemak/update] Patching %s from %s to %s, with staging dir at %s. ", folder_name.c_str(), km_installed_version.value().version.c_str(), get_latest_version_code().c_str(),
             staging_path.c_str());
    int patch_ret = butler_patch(patch_url_full.str(), staging_path.string(), patch.value().filename, game_path.string(), patch.value().temp_required);
    if(patch_ret != 0)
    {
        A_error(ErrorLevel::SERIOUS,"Patching failed - error code %d\n",patch_ret);
        return patch_ret;
    }
    installed_version_code = get_latest_version_code();
    write_version();
    return 0;
}

int Kachemak::install(std::filesystem::path path)
{
    std::optional<KachemakVersion> latest_version = get_latest_km_version();
    if (!latest_version)
    {
        return 2;
    }
    int disk_space_status = free_space_check(latest_version.value().download_size, FreeSpaceCheckCategory::Temporary,path);
    if (disk_space_status != 0)
    {
        // should we error here?
        return disk_space_status;
    }
    std::string download_uri = source_url + latest_version.value().download_url;
    A_printf("[Kachemak/install] Downloading via torrent...");
    int download_status = torrent::libtorrent_download(download_uri, temp_path.string(), &event_system);
    if (download_status != 0)
    {
        A_error(ErrorLevel::SERIOUS,"[Kachemak/install] Download failed - ret val %d \n", download_status);
        return download_status;
    }
    A_printf("[Kachemak/install] Download complete: extracting...");
    std::filesystem::create_directory(path.string() / folder_name);
    int ret = extract(latest_version.value().file_name, (path / folder_name).string(), latest_version.value().extract_size);
    if (ret == 0)
    {
        A_printf("[Kachemak/install] Extraction done!");
        installed_version_code = get_latest_version_code();
        write_version();
        return 0;
    }
    return 1;
}

/**
desc:
        extract .zip file to directory
res:
        0: success
        1: not enough free space
*/
int Kachemak::extract(const std::string &input_file, const std::string &output_directory, const size_t &size)
{
    A_printf("[Kachemak/extract] Input File %s, Output %s, size %lu", input_file.c_str(), output_directory.c_str(), size);
    if (free_space_check(size * 2, FreeSpaceCheckCategory::Permanent,output_directory) != 0)
    {
        A_printf("[Kachemak/extract] Not enough space. Exiting.");
        return 1;
    }
    int ret = sys::extract_zip((temp_path / input_file).string(), output_directory);
    if (ret != 0)
    {
        A_error(ErrorLevel::SERIOUS,"[Kachemak/extract] Extraction Failed - %s\n", zip_strerror(ret));
        return -1;
    }
    installed_version_code = get_latest_version_code();
    write_version();
    return 0;
}

/**
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
                ErrorMessage message(json_buffer["message"].get<std::string>(),ErrorLevel::PANIC);
                event_system.trigger_event(message);
            }
        }
    }

    pclose(pipe);

    return 0;
}
void Kachemak::find_installed_version()
{
    if (exists(game_path))
    {
        std::ifstream data(game_path / ".adastral");
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
    std::ofstream data(game_path / ".adastral");
    data << test_json;
    data.close();
}
