#include "palace.hpp"

palace::palace() {
  A_printf("[Palace/Init] Fetching server data...\n");
  fetch_server_data();
  download_assets();

#if _DEBUG
  printf("Soucemods dir: %s\n", sourcemodsPath.string().c_str());
#endif
}
palace::~palace() {
  for (const auto& it : serverGames) {
    delete it.second;
  }
}



void palace::fetch_server_data() {
  std::string json = net().get_string_data_from_server(std::string(PRIMARY_URL) + "southbank2.json");
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
  std::unordered_map<std::string,std::string> initmap = {
    {BUTLER, std::string(BUTLER) + ".sha256"}
  };

  for (auto& item : initmap) {
    std::string local_file_path = (appdata_path / item.first).string();
    std::string local_hash = A_SHA256(local_file_path);
    std::string server_hash = net().get_string_data_from_server(std::string(PRIMARY_URL) + item.second);
    cachemap[item.first] = local_file_path; // very naughty
    if (strncmp(local_hash.c_str(), server_hash.c_str(), 64) == NULL) continue;
    net().download_to_temp(std::string(PRIMARY_URL) + item.first, local_file_path);
    A_printf("%s | %s",item.first.c_str(),local_file_path.c_str());
#if _DEBUG
    printf("Downloading asset from cache lookup: %s\n", item.second.c_str());
#endif
  }
#ifndef GODOT
#else
  for (const auto& game : southbankJson["games"].items()) {
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

std::filesystem::path palace::get_asset(std::string hash) {
    return cachemap[hash];
}

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

bool palace::isTF2Installed() {
  if (sourcemodsPath != "") {
    TF2Installed = sys::CheckTF2Installed(sourcemodsPath.parent_path().parent_path());
    return TF2Installed;
  }
  return false;
}

bool palace::isSDKInstalled() {
  if (sourcemodsPath != "") {
    TF2Installed = sys::CheckTF2Installed(sourcemodsPath.parent_path().parent_path());
    return SDKInstalled;
  }
  return false;
}

int palace::init_games() {
  if (!std::filesystem::exists(std::filesystem::canonical(sourcemodsPath))) {
    return 2;
  }
  for (const auto& it : southbankJson["games"].items()) {
    std::string version;
    std::string id = it.key();
    serverGames[id] = new GameMetadata;
    serverGames[id]->name = it.value()["name"];
    std::string full_url = southbankJson["dl_url"];
    full_url += id;
    full_url += '/';  // this is dumb, make it do this inside kachemak....
    auto* game = new Kachemak(sourcemodsPath, it.key(),
                              full_url,get_asset(BUTLER));  // getting the json is versioning impl specific so we let it get it
    // i'm aware i'm breaking one of the rules, but it makes more sense
    serverGames[id]->l1 = game;
  }
  return 0;
}

int palace::update_game(const std::string& game_name) {
  A_printf("[Palace/UpdateGame] Updating %s....",game_name.c_str());
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
