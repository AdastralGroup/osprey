#include "palace.hpp"

palace::palace() {
  fremont().sanity_checks();
  sourcemodsPath = fremont().GetSteamSourcemodPath();
  if(sourcemodsPath == ""){
      std::cout << "NO SOURCEMOD PATH!" << std::endl;
  }
  fremont::get_butler();
  fetch_server_data();
#if _DEBUG
    printf("Soucemods dir: %s\n", sourcemodsPath.string().c_str());
#endif
}

void palace::fetch_server_data() {
  southbankJson = nlohmann::json::parse(
      fremont().get_string_data_from_server(std::string(PRIMARY_URL) + "southbank.json"));  // do error checking here
}


int palace::init_games() {
  if (!std::filesystem::exists(std::filesystem::canonical(sourcemodsPath)))
  {
    return 2;
  }

  for(const auto& it: southbankJson["games"].items()){
    std::string version;
    if(std::filesystem::exists(sourcemodsPath / it.key())) {
      std::cout << it.key() << ": Game exists. " <<std::endl;
      std::ifstream data(sourcemodsPath / it.key() / ".adastral");
      if (!data.fail()) {
        nlohmann::json filedata = nlohmann::json::parse(data);
        version = filedata["version"];
      }
      std::cout << "Adastral supported game detected, but adastral spec not detected. Continuing.";
    }
      std::string full_url = southbankJson["dl_url"];
      full_url += it.key();
      full_url += '/'; // this is dumb, make it do this inside kachemak....
      auto* game = new Kachemak(sourcemodsPath,it.key(),full_url,version); // getting the json is versioning impl specific so we let it get it
      // i'm aware i'm breaking one of the rules, but it makes more sense
      serverGames[it.key()] = game;
  }
  return 0;
}

int palace::update_game(const std::string& game_name) {
  if(serverGames[game_name]->GetInstalledVersion().empty()){
      serverGames[game_name]->Install();
  }else{
    serverGames[game_name]->Update();
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
