#include <palace/palace.hpp>

int main() {
  auto p = new palace; // does sanity checks
  p->get_server_games();
  int code = p->init_games();

  switch (code)
  {
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

  p->update_game("open_fortress");
  return 0;
}