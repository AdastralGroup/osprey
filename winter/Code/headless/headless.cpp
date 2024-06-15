#include "headless.h"
int main() {
  printf(WELCOME_TEXT);
  auto p = new palace;  // does sanity checks
  A_init_error_system();
  A_error_system->RegisterListener(EventType::kOnError, [](Event& ev) {
    printf(static_cast<ErrorMessage&>(ev).get_message().c_str());
  });
  p->find_sourcemod_path();
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
  for (const auto& i : p->serverGames) {
    std::cout << "[headless] "
              << "Game Available: " << i.second->name << " (" << i.first << ")" << std::endl;
  }
  printf("[headless] Updating open_fortress.\n");
  p->update_game("open_fortress");  // you should check the force_verify to see if verification alone is needed.
  delete(p);
  printf("[headless] Everything done.\n");
  return 0;
}