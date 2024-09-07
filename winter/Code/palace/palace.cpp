#include "palace.hpp"

palace::palace(std::string path)
{
    if (path == "")
    {
        if (sys::get_steam_path() == std::filesystem::path(""))
        {
            throw std::runtime_error("Failed to obtain steam path! reinit palace with correct path manually.");
        }
        steam_path = sys::get_steam_path();
    }
    else
    {
        steam_path = std::filesystem::path(path);
    }
    A_printf("[Palace/Init] Fetching server data...");
    fetch_server_data();
    A_printf("[Palace/Init] Downloading assets... ");
    download_assets();
    library_folders = sys::parse_vdf_file(steam_path / "steamapps" / "libraryfolders.vdf");
#if __unix__
    config_file = sys::parse_vdf_file(steam_path / "config" / "config.vdf");
#endif
}

palace::~palace()
{
    for (const auto &it : server_games)
    {
        delete it.second;
    }
}

void palace::fetch_server_data()
{
    std::string json = net().get_string_data_from_server(SB_URL);
    try
    {
        southbank_json = nlohmann::json::parse(json); // do error checking here
    }
    catch (nlohmann::json::parse_error &ex)
    {
        A_error("Can't fetch southbank. Check your internet connection!");
    }
}

void palace::download_assets()
{
#ifdef _WIN32
    std::filesystem::path appdata_path = std::filesystem::path(getenv("APPDATA")) / "adastral";
#else
    std::filesystem::path appdata_path = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "adastral";
#endif
    if (!std::filesystem::exists(appdata_path))
    {
        std::filesystem::create_directory(appdata_path);
    }
    std::unordered_map<std::string, std::string> initmap = {{BUTLER, std::string(BUTLER) + ".sha256"}};
    for (auto &item : initmap)
    {
        std::string local_file_path = (appdata_path / item.first).string();
        std::string local_hash = A_SHA256(local_file_path);
        std::string server_hash = net().get_string_data_from_server(std::string(ROOT_URL) + item.second);
        cachemap[item.first] = local_file_path; // very naughty
        if (strncmp(local_hash.c_str(), server_hash.c_str(), 64) == NULL)
        {
            continue;
        }
        net().download_to_temp(std::string(ROOT_URL) + item.first, local_file_path);
        A_printf("%s | %s", item.first.c_str(), local_file_path.c_str());
#if _DEBUG
        printf("Downloading asset from cache lookup: %s\n", item.second.c_str());
#endif
    }
#ifndef GODOT
#else
    for (const auto &game : southbank_json["games"].items())
    {
        if (game.key().find("$") != std::string::npos)
        {
            continue;
        }
        std::filesystem::path game_asset_dir = appdata_path / game.key();
        if (!std::filesystem::exists(game_asset_dir))
        {
            std::filesystem::create_directory(game_asset_dir);
        }
        for (const auto &belmont_item : game.value()["belmont"].items())
        {
            bool is_asset = belmont_item.value().is_array(); // Southbank holds colors & assets in the same object, by looking if it is an array we can tell if it has an sha256 value or not
            if (!is_asset)
            {
                continue;
            }

            std::string file_url = belmont_item.value()[0].get<std::string>();
            std::string file_extension = file_url.substr(file_url.find_last_of("."));
            std::string server_file_checksum = belmont_item.value()[1].get<std::string>();
            std::filesystem::path local_file_path = game_asset_dir / (belmont_item.key() + file_extension);
            std::string local_file_checksum = A_SHA256(local_file_path.string());
            cachemap[server_file_checksum] = local_file_path.string();
            if (strncmp(local_file_checksum.c_str(), server_file_checksum.c_str(), 64) == NULL)
            {
                continue;
            }
            net().download_to_temp(file_url, local_file_path.string());
#if _DEBUG
            printf("Downloading game asset: %s\n", file_url.c_str());
#endif
        }
    }
#endif
}

std::filesystem::path palace::get_asset(std::string hash)
{
    return cachemap[hash];
}

std::filesystem::path palace::find_sourcemod_path()
{
    if (steam_path != "")
    {
        A_printf("[Palace] Steam Path found!");
        sourcemods_path = steam_path / "steamapps" / "sourcemods";
        if (std::filesystem::exists(sourcemods_path))
        {
            A_printf("[Palace] Sourcemod folder exists");
            return sourcemods_path;
        }
        else
        {
            A_printf("[Palace] Sourcemod folder doesn't exist - creating...");
            std::filesystem::create_directories(sourcemods_path);
        }
    }
    return sourcemods_path;
}

int palace::init_games()
{
    A_printf("[Palace/InitGames] Initialising games...");
    if (!std::filesystem::exists(std::filesystem::canonical(sourcemods_path)))
    {
        return 2;
    }
    for (const auto &it : southbank_json["games"].items())
    {
        std::string version;
        std::string id = it.key();
        if (id.find("$") != std::string::npos)
        {
            continue;
        }
        server_games[id] = new GameMetadata;
        server_games[id]->name = it.value()["name"];
        std::string full_url = southbank_json["dl_url"];
        full_url += id;
        full_url += '/'; // this is dumb, make it do this inside kachemak....
        auto *game = new Kachemak(sourcemods_path, it.key(), full_url, get_asset(BUTLER));
        // getting the json is versioning impl specific so we let it get it
        // i'm aware i'm breaking one of the rules, but it makes more sense
        server_games[id]->l1 = game;
    }
    return 0;
}

int palace::update_game(const std::string &game_name)
{
    A_printf("[Palace/UpdateGame] Updating %s....", game_name.c_str());
    if (server_games[game_name]->l1->get_installed_version_code().empty())
    {
        server_games[game_name]->l1->install();
    }
    // else if(server_games[game_name]->l1->get_installed_version() == server_games[game_name]->l1->GetLatestVersion() ||
    // server_games[game_name]->l1->force_verify){
    //   server_games[game_name]->l1->verify();
    // }
    else
    {
        return server_games[game_name]->l1->update();
    }
    return 0;
}

// creating the same function to accept in custom path names.
int palace::update_game_with_path(const std::string &game_name, const std::string custom_path)
{
    A_printf("[Palace/UpdateGameWithPath] Updating %s....", game_name.c_str());

    // First, we sanitize the path and try to convert it to std::filesystem::path variable.
    const std::filesystem::path sanitized_path = std::filesystem::u8path(custom_path); // windows-specific thing that may work on linux, need to try on that

    // Then we practically do the same thing except inserting the sanitized path to the overloaded install function.
    if (server_games[game_name]->l1->get_installed_version_code().empty())
    {
        server_games[game_name]->l1->install_path(sanitized_path);
    }
    // else if(server_games[game_name]->l1->get_installed_version() == server_games[game_name]->l1->GetLatestVersion() ||
    // server_games[game_name]->l1->force_verify){
    //   server_games[game_name]->l1->verify();
    // }
    // else {
    //  server_games[game_name]->l1->update();
    //}
    return 0;
}
std::vector<std::string> palace::get_games()
{
    auto vec = std::vector<std::string>();
    for (const auto &it : server_games)
    {
        vec.push_back(it.first);
    }
    return vec;
}
int palace::verify_game(const std::string &game_name)
{
    server_games[game_name]->l1->verify();
    return 0;
}

bool palace::is_app_installed(const std::string &app_id)
{
    for (auto directory : library_folders.childs)
    {
        auto apps = directory.second->childs["apps"];
        if (apps->attribs.find(app_id) != apps->attribs.end())
        {
            return true;
        };
    }
    return false;
}

std::filesystem::path palace::get_app_path(const std::string &app_id)
{
    for (auto directory : library_folders.childs)
    {
        auto apps = directory.second->childs["apps"];
        if (apps->attribs.find(app_id) != apps->attribs.end())
        { // if the app is found in this specific depot
            std::filesystem::path apps_path = std::filesystem::path(directory.second->attribs["path"]) / "steamapps"; // where the app's installed ("libraryfolders" -> [incrementing id] -> path)
            std::string manifest_file = "appmanifest_" + app_id + ".acf";
            auto kv_parsed = sys::parse_vdf_file(apps_path / manifest_file);
            return apps_path / "common" / kv_parsed.attribs["installdir"];
        }
    }
    return std::filesystem::path(); // it's not there
}

#define SOURCE_SDK_2013_APP_ID "243750"
// returns non-zero if failed
int palace::launch_game(const std::string &game_name, const std::string &arguments)
{
    std::filesystem::path sdk_app_path = get_app_path(SOURCE_SDK_2013_APP_ID);
    if (!std::filesystem::exists(sdk_app_path))
    {
        A_error("[Palace/launch_game] sdk path doesn't exist...");
        return 1;
    }
#ifdef WIN32
    std::string sdk_app_binary = (sdk_app_path / "hl2.exe").string();
    char *command_line = new char[1024]; // yeah i don't think anyone needs more
    snprintf(command_line, 1024, "%s -game sourcetest -steam -game \"%s\" -steam -secure %s", sdk_app_binary.c_str(), (sourcemods_path / game_name).string().c_str(), arguments.c_str());
    STARTUPINFO dummy_si = {0};
    dummy_si.cb = sizeof(dummy_si);
    PROCESS_INFORMATION dummy_pi = {0};

    if (CreateProcessA(sdk_app_binary.c_str(), command_line, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, (LPSTR)sourcemods_path.string().c_str(), &dummy_si, &dummy_pi) == 0)
    {
        A_error("[Palace/launch_game] win32: CreateProcessA failed!");
        return 1;
    }
#else
    std::string url_launch = "steam -applaunch ";
    url_launch += SOURCE_SDK_2013_APP_ID;
    url_launch += " -game ";
    url_launch += (sourcemods_path / game_name).string();
    url_launch += " ";
    url_launch += arguments;
    A_printf("%s",url_launch.c_str());
    if (popen(url_launch.c_str(), "r") == NULL)
    {
        return 1;
    }
#endif

    return 0;
}