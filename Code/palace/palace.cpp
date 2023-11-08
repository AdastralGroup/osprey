#include "palace.hpp"

palace::palace() {
  A_printf("[Palace/Init] Downloading butler. Hold on.\n");
  fremont::get_butler();
  A_printf("[Palace/Init] Fetching server data...\n");
  fetch_server_data();
#if _DEBUG
    printf("Soucemods dir: %s\n", sourcemodsPath.string().c_str());
#endif
}

void palace::fetch_server_data() {
    std::string json = fremont().get_string_data_from_server(std::string(PRIMARY_URL) + "southbank.json");
  southbankJson = nlohmann::json::parse(json);  // do error checking here
}

std::filesystem::path palace::find_sourcemod_path(){
  std::filesystem::path steamPath = fremont().GetSteamPath();
  if(steamPath != ""){
    A_printf("[Palace] Steam Path found!\n");
    sourcemodsPath = steamPath / "steamapps" / "sourcemods";
    if(std::filesystem::exists(sourcemodsPath)) {
      A_printf("[Palace] Sourcemod folder exists\n");
      return sourcemodsPath;
    }else{
      A_printf("[Palace] Sourcemod folder doesn't exist - creating...\n");
      std::filesystem::create_directories(sourcemodsPath);
    }
  }else{
    A_printf("[Palace] NO STEAM PATH?!\n");
  }
  return sourcemodsPath;
}

bool palace::isTF2Installed(){
  if(sourcemodsPath != "") {
    TF2Installed = fremont::CheckTF2Installed(sourcemodsPath.parent_path().parent_path());
    return TF2Installed;
  }
  return false;
}

bool palace::isSDKInstalled(){
  if(sourcemodsPath != "") {
    TF2Installed = fremont::CheckTF2Installed(sourcemodsPath.parent_path().parent_path());
    return SDKInstalled;
  }
  return false;
}

int palace::init_games() {
  if (!std::filesystem::exists(std::filesystem::canonical(sourcemodsPath)))
  {
    return 2;
  }
  for(const auto& it: southbankJson["games"].items()){
    std::string version;
    std::string id = it.key();
    if(std::filesystem::exists(sourcemodsPath / id)) {
      A_printf("[Palace] %s: Game exists.\n",id.c_str());
    }
      serverGames[id] = new GameMetadata;
      serverGames[id]->name = it.value()["name"];
      std::string full_url = southbankJson["dl_url"];
      full_url += id;
      full_url += '/'; // this is dumb, make it do this inside kachemak....
      auto* game = new Kachemak(sourcemodsPath,it.key(),full_url); // getting the json is versioning impl specific so we let it get it
      // i'm aware i'm breaking one of the rules, but it makes more sense
      serverGames[id]->l1 = game;
  }
  return 0;
}

int palace::update_game(const std::string& game_name) {
  if(serverGames[game_name]->l1->GetInstalledVersion().empty()){
      serverGames[game_name]->l1->Install();
  }
  //else if(serverGames[game_name]->l1->GetInstalledVersion() == serverGames[game_name]->l1->GetLatestVersion() || serverGames[game_name]->l1->force_verify){
  //  serverGames[game_name]->l1->Verify();
  //}
  else{
    serverGames[game_name]->l1->Update();
  }
  return 0;
}
std::vector<std::string> palace::get_games() {
  auto vec = std::vector<std::string>();
  for(const auto& it: serverGames){
      vec.push_back(it.first);
  }
  return vec;
}
int palace::verify_game(const std::string& gameName) {
  serverGames[gameName]->l1->Verify();
  return 0;
}
