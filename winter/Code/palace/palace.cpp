#include "palace.hpp"


palace::palace(std::string path) {
  if(path == "") {
    if(sys::GetSteamPath() == std::filesystem::path("")) {
      throw std::runtime_error("Failed to obtain steam path! reinit palace with correct path manually.");
    }
    steamPath = sys::GetSteamPath();
  }else {
    steamPath = std::filesystem::path(path);
  }
  A_printf("[Palace/Init] Fetching server data...");
  fetch_server_data();
  A_printf("[Palace/Init] Downloading assets... ");
  download_assets();
  library_folders = sys::ParseVDFFile(steamPath/ "steamapps" / "libraryfolders.vdf");
#if __unix__
  config_file = sys::ParseVDFFile(steamPath/ "config" / "config.vdf");
#endif
}



palace::~palace() {
  for (const auto& it : serverGames) {
    delete it.second;
  }
}

void palace::fetch_server_data() { // function is separate so we can grab the server data again if needs be
  std::string json = net().get_string_data_from_server(SB_URL);
  try
  {
    southbankJson = nlohmann::json::parse(json);  // do error checking here
  }
  catch (nlohmann::json::parse_error& ex)
  {
    A_error("Can't fetch southbank. Check your internet connection!");
  }
}

void palace::download_assets() {
#ifdef _WIN32
  std::filesystem::path appdata_path = std::filesystem::path(getenv("APPDATA")) / "adastral";
#else
  std::filesystem::path appdata_path = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "adastral";
#endif
  if (!std::filesystem::exists(appdata_path)) {
    std::filesystem::create_directory(appdata_path);
  }
  std::unordered_map<std::string, std::string> initmap = {{BUTLER, std::string(BUTLER) + ".sha256"}};

  for (auto& item : initmap) {
    std::string local_file_path = (appdata_path / item.first).string();
    std::string local_hash = A_SHA256(local_file_path);
    std::string server_hash = net().get_string_data_from_server(std::string(ROOT_URL) + item.second);
    cachemap[item.first] = local_file_path;  // very naughty
    if (strncmp(local_hash.c_str(), server_hash.c_str(), 64) == NULL) continue;
    net().download_to_temp(std::string(ROOT_URL) + item.first, local_file_path);
    A_printf("%s | %s", item.first.c_str(), local_file_path.c_str());
#if _DEBUG
    printf("Downloading asset from cache lookup: %s\n", item.second.c_str());
#endif
  }
#ifndef GODOT
#else
  for (const auto& game : southbankJson["games"].items()) {
    if(game.key().find("$") != std::string::npos){continue;}
    std::filesystem::path game_asset_dir = appdata_path / game.key();
    if (!std::filesystem::exists(game_asset_dir)) {
      std::filesystem::create_directory(game_asset_dir);
    }
    for (const auto& belmont_item : game.value()["belmont"].items()) {
      bool is_asset =
          belmont_item.value().is_array();  // Southbank holds colors & assets in the same object, by looking if it is
                                            // an array we can tell if it has an sha256 value or not
      if (!is_asset) continue;

      std::string file_url = belmont_item.value()[0].get<std::string>();
      std::string file_extension = file_url.substr(file_url.find_last_of("."));
      std::string server_file_checksum = belmont_item.value()[1].get<std::string>();
      std::filesystem::path local_file_path = game_asset_dir / (belmont_item.key() + file_extension);
      std::string local_file_checksum = A_SHA256(local_file_path.string());
      cachemap[server_file_checksum] = local_file_path.string();
      if (strncmp(local_file_checksum.c_str(), server_file_checksum.c_str(), 64) == NULL) continue;
      net().download_to_temp(file_url, local_file_path.string());
#if _DEBUG
      printf("Downloading game asset: %s\n", file_url.c_str());
#endif
    }
  }
#endif
}

std::filesystem::path palace::get_asset(std::string hash) { return cachemap[hash]; }

std::filesystem::path palace::find_sourcemod_path() {
  if (steamPath != "") {
    A_printf("[Palace] Steam Path found!");
    sourcemodsPath = steamPath / "steamapps" / "sourcemods";
    if (std::filesystem::exists(sourcemodsPath)) {
      A_printf("[Palace] Sourcemod folder exists");
      return sourcemodsPath;
    } else {
      A_printf("[Palace] Sourcemod folder doesn't exist - creating...");
      std::filesystem::create_directories(sourcemodsPath);
    }
  }
  return sourcemodsPath;
}

int palace::init_games() {
  A_printf("[Palace/InitGames] Initialising games...");
  if (!std::filesystem::exists(std::filesystem::canonical(sourcemodsPath))) {
    return 2;
  }
  for (const auto& it : southbankJson["games"].items()) {
    std::string version;
    std::string id = it.key();
    if(id.find("$") != std::string::npos){continue;}
    serverGames[id] = new GameMetadata;
    serverGames[id]->name = it.value()["name"];
    std::string full_url = southbankJson["dl_url"];
    full_url += id;
    full_url += '/';  // this is dumb, make it do this inside kachemak....
    auto* game = new Kachemak(sourcemodsPath, it.key(), full_url,
                              get_asset(BUTLER));  // getting the json is versioning impl specific so we let it get it
    // i'm aware i'm breaking one of the rules, but it makes more sense
    serverGames[id]->l1 = game;
  }
  return 0;
}

int palace::update_game(const std::string& game_name) {
  A_printf("[Palace/UpdateGame] Updating %s....", game_name.c_str());
  if (serverGames[game_name]->l1->GetInstalledVersion().empty()) {
    serverGames[game_name]->l1->Install();
  }
  // else if(serverGames[game_name]->l1->GetInstalledVersion() == serverGames[game_name]->l1->GetLatestVersion() ||
  // serverGames[game_name]->l1->force_verify){
  //   serverGames[game_name]->l1->Verify();
  // }
  else {
    serverGames[game_name]->l1->Update();
  }
  return 0;
}

// creating the same function to accept in custom path names.
int palace::update_game_with_path(const std::string& game_name, const std::string customPath) {
  A_printf("[Palace/UpdateGameWithPath] Updating %s....", game_name.c_str());

  // First, we sanitize the path and try to convert it to std::filesystem::path variable.
  const std::filesystem::path sanitizedPath =
      std::filesystem::u8path(customPath);  // windows-specific thing that may work on linux, need to try on that

  // Then we practically do the same thing except inserting the sanitized path to the overloaded Install function.
  if (serverGames[game_name]->l1->GetInstalledVersion().empty()) {
    serverGames[game_name]->l1->Install_InPath(sanitizedPath);
  }
  // else if(serverGames[game_name]->l1->GetInstalledVersion() == serverGames[game_name]->l1->GetLatestVersion() ||
  // serverGames[game_name]->l1->force_verify){
  //   serverGames[game_name]->l1->Verify();
  // }
  // else {
  //  serverGames[game_name]->l1->Update();
  //}
  return 0;
}
std::vector<std::string> palace::get_games() {
  auto vec = std::vector<std::string>();
  for (const auto& it : serverGames) {
    vec.push_back(it.first);
  }
  return vec;
}
int palace::verify_game(const std::string& gameName) {
  serverGames[gameName]->l1->Verify();
  return 0;
}

bool palace::is_app_installed(const std::string& app_id) {
  for(auto directory: library_folders.childs){
    auto apps = directory.second->childs["apps"];
    if (apps->attribs.find(app_id) != apps->attribs.end()) {
      return true;
    };
  }
  return false;
}

std::filesystem::path palace::get_app_path(const std::string& app_id) {
  for(auto directory: library_folders.childs){
    auto apps = directory.second->childs["apps"];
    if (apps->attribs.find(app_id) != apps->attribs.end()) { // if the app is found in this specific depot
      std::filesystem::path apps_path = std::filesystem::path(directory.second->attribs["path"]) / "steamapps"; // where the app's installed ("libraryfolders" -> [incrementing id] -> path)
      std::string manifest_file = "appmanifest_" + app_id + ".acf";
      auto kv_parsed = sys::ParseVDFFile(apps_path / manifest_file);
      return apps_path / "common" / kv_parsed.attribs["installdir"];
    }
  }
  return std::filesystem::path(); // it's not there
}

#define SOURCE_SDK_2013_APP_ID "243750"

// no idea if there is somewhere an map like that in my steam directory and i don't wanna read every app manifest / call steam api over this silly thing 
const std::map<std::string, std::string> proton_map_to_depot = {
  {"proton_experimental", "1493710"},
  {"proton_9", "2805730"}
};

// returns non-zero if failed
int palace::launch_game(const std::string& game_name, const std::string& arguments) {
  std::string last_args = sys::GetLaunchArgsForRecentUser(steamPath,"243750_3661242217");
  sys::SetLaunchArgsForRecentUser(steamPath,std::string(SOURCE_SDK_2013_APP_ID),arguments);
#ifdef WIN32
  std::string run_cmd = "start steam://run/" + std::string(SOURCE_SDK_2013_APP_ID);
#else
  std::string run_cmd = "xdg-open steam://run/" + std::string(SOURCE_SDK_2013_APP_ID);
#endif
#ifdef WIN32
  system(run_cmd.c_str())
  }
#else
  popen(run_cmd.c_str(), "r") == NULL;
#endif
  sys::SetLaunchArgsForRecentUser(steamPath,std::string(SOURCE_SDK_2013_APP_ID),last_args);
  return 0;
}