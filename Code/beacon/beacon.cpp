#include <palace/palace.hpp>

int main() {
  auto p = new palace; // does sanity checks
  p->get_server_games();
  if(p->init_games()==1){
    printf("non-adastral game in folder\n");
  };
  p->update_game("open_fortress");
  return 0;
}