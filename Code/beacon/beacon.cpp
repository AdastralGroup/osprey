#include "beacon.h"
int main() {
  printf(WELCOME_TEXT);
  auto p = new palace;  // does sanity checks
  if (p->sourcemodsPath.empty()) {
    printf("We couldn't find your sourcemod folder. Please input an alternate sourcemods path.\n");
  try_again:
    std::string path;
    getline(std::cin, path);
    if (!std::filesystem::exists(path)) {
      printf("That's not a valid path. Probably. Try again. Note the folder needs to exist.\n");
      goto try_again;
    }
    p->sourcemodsPath = std::filesystem::path(path);
  }

  int code = p->init_games();
  printf("[GameInit] ");
  switch (code) {
    case 0:
      printf("Initialization success!\n");
      break;
    case 1:
      printf("non-adastral game in folder\n");
      break;
    case 2:
      printf("Steam sourcemods dir was not found. Is steam installed?\n");
      return code;
  }
  p->fetch_server_data();
  for (const auto& i : p->serverGames){
    std::cout << "[Beacon] " << "Game Available: " << i.second->name << " ("<< i.first << ")"<< std::endl;
  }
  printf("[Beacon] Updating tf2classic.\n");
  p->verify_game("tf2classic"); // you should check the force_verify to see if verification alone is needed.
  return 0;
}