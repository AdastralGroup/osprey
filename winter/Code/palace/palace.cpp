#include "palace.hpp"
#include <cstdlib>
#include <map>
#include "KeyValue.h"
#include "adastral_defs.h"

palace::palace() {
  A_printf("[Palace/Init] Fetching server data...\n");
  fetch_server_data();
  A_printf("[Palace/Init] Downloading assets... \n");
  download_assets();
  library_folders = sys::ParseVDFFile(sys::GetSteamPath() / "steamapps" / "libraryfolders.vdf");
#if __unix__
  config_file = sys::ParseVDFFile(sys::GetSteamPath() / "config" / "config.vdf");
#endif
}

palace::~palace() {
  for (const auto& it : serverGames) {
    delete it.second;
  }
  delete library_folders;
}

void palace::fetch_server_data() {
  std::string json = net().get_string_data_from_server(SB_URL);
  southbankJson = nlohmann::json::parse(json);  // do error checking here
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
  std::filesystem::path steamPath = sys::GetSteamPath();
  if (steamPath != "") {
    A_printf("[Palace] Steam Path found!\n");
    sourcemodsPath = steamPath / "steamapps" / "sourcemods";
    if (std::filesystem::exists(sourcemodsPath)) {
      A_printf("[Palace] Sourcemod folder exists\n");
      return sourcemodsPath;
    } else {
      A_printf("[Palace] Sourcemod folder doesn't exist - creating...\n");
      std::filesystem::create_directories(sourcemodsPath);
    }
  } else {
    A_printf("[Palace] NO STEAM PATH?!\n");
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

  // We assign in the symlink first before doing the update first, as we assume its already installed, just in a different path
  if (serverGames[game_name]->l1->GetInstalledVersion().empty()) {
    //serverGames[game_name]->l1->Install_InPath(sanitizedPath);
    serverGames[game_name]->l1->CreateSymlink(sanitizedPath);
    
  }
  
  if(serverGames[game_name]->l1->GetInstalledVersion() == serverGames[game_name]->l1->GetLatestVersion() ||
    serverGames[game_name]->l1->force_verify){
    serverGames[game_name]->l1->Verify();
   }
   else {
    serverGames[game_name]->l1->Update();
  }
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
  KeyValue* directory = library_folders->Children()->Children();
  while (directory && directory->IsValid()) {
    KeyValue& apps = directory->Get("apps");
    KeyValue& app = apps.Get(app_id.c_str());

    if (app.IsValid()) return true;

    directory = directory->Next();
  }

  return false;
}

std::filesystem::path palace::get_app_path(const std::string& app_id) {
  KeyValue* directory = library_folders->Children()->Children();

  while (directory && directory->IsValid()) {
    KeyValue& apps = directory->Get("apps");
    KeyValue& app = apps.Get(app_id.c_str());

    if (app.IsValid()) {
      std::filesystem::path apps_path = std::filesystem::path(directory->Get("path").Value().string) / "steamapps";
      std::string manifest_file = "appmanifest_" + app_id + ".acf";
      KeyValueRoot* kv_parsed = sys::ParseVDFFile(apps_path / manifest_file);
      kvString_t install_dir = kv_parsed->Children()->Get("installdir").Value();
      std::filesystem::path retval = apps_path / "common" / std::filesystem::path(install_dir.string);
      delete kv_parsed;
      return retval;
    }

    directory = directory->Next();
  }

  return std::filesystem::path();
}

#define SOURCE_SDK_2013_APP_ID "243750"

// no idea if there is somewhere an map like that in my steam directory and i don't wanna read every app manifest / call steam api over this silly thing 
const std::map<std::string, std::string> proton_map_to_depot = {
  {"proton_experimental", "1493710"}
};

// returns non-zero if failed
int palace::launch_game(const std::string& game_name, const std::string& arguments) {
  std::filesystem::path sdk_app_path = get_app_path(SOURCE_SDK_2013_APP_ID);
  if (!std::filesystem::exists(sdk_app_path)) {
    A_error("[Palace/launch_game] sdk path doesn't exist...");
    return 1;
  }
#ifdef WIN32
  std::string sdk_app_binary = (sdk_app_path / "hl2.exe").string();
#else
  KeyValue& source_sdk_compat_tool = config_file->Children()->Children()->Children()->Children()->Get("CompatToolMapping").Get(SOURCE_SDK_2013_APP_ID); 
  /* "InstallConfigStore"->"Software"->"Valve"->"Steam"->CompatToolMapping */
  std::string sdk_app_binary;
  if (source_sdk_compat_tool.IsValid()) {
    return 1; // proton doesn't work for now.
    const char* proton_app_name = source_sdk_compat_tool.Get("name").Value().string;
    if (!proton_map_to_depot.contains(proton_app_name)) {
      A_error("%s proton version couldn't be found\n", proton_app_name);
      return 1;
    }
    setenv("STEAM_COMPAT_DATA_PATH", (sys::GetSteamPath() / "steamapps" / "compatdata").string().c_str(), 1);
    setenv("STEAM_COMPAT_CLIENT_INSTALL_PATH", sys::GetSteamPath().string().c_str(), 1);
    std::filesystem::path proton_app_path = get_app_path(proton_map_to_depot.at(proton_app_name)); 
    sdk_app_binary.append("python3 \"" + proton_app_path.string() + "/\"proton waitforexitandrun \"" + (sdk_app_path / "hl2.exe\"").string());
  } else {
    sdk_app_binary.append((sdk_app_path / "hl2.sh").string());
  }
#endif
  char* command_line = new char[1024];  // yeah i don't think anyone needs more
  snprintf(command_line, 1024, "%s -game sourcetest -steam -game \"%s\" -steam -secure %s", sdk_app_binary.c_str(),
           (sourcemodsPath / game_name).string().c_str(), arguments.c_str());
#ifdef WIN32
  STARTUPINFO dummy_si = {0};
  dummy_si.cb = sizeof(dummy_si);
  PROCESS_INFORMATION dummy_pi = {0};

  if (CreateProcessA(sdk_app_binary.c_str(), command_line, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL,
                     (LPSTR)sourcemodsPath.string().c_str(), &dummy_si, &dummy_pi) == 0) {

    A_error("[Palace/launch_game] win32: CreateProcessA failed!");
    return 1;
  }
#else
  setenv("SteamEnv", "1", 1);
  if (popen(command_line, "r") == NULL) return 1;
#endif

  return 0;
}