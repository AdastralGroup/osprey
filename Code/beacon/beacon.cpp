#include <palace/palace.hpp>

int main() {
  auto p = new palace; // does sanity checks
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

  p->fetch_server_data();
  for (const auto& i: p->get_games())
    std::cout<<i<<std::endl;
  p->update_game(p->get_games()[0]);
  return 0;
}