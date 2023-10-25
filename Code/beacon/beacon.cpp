#include <palace/palace.h>

int main() {
  auto p = new palace;
  std::cout << p->sanity_checks() << std::endl;
  p->get_server_games();
  return 0;
}