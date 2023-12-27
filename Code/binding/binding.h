#include <adastral_defs.h>

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <palace.hpp>
#include <thread>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#else
#include <libnotify/notify.h>
#endif

namespace godot {
class binding : public Node {
  GDCLASS(binding, Node);

 public:
  binding();
  ~binding();
  void init_palace();
  static void _bind_methods();
  int sanity_checks();
  godot::Dictionary get_game_assets(godot::String game_name);
  godot::Array get_server_games();
  int update_game(godot::String gameName);
  int verify_game(godot::String gameName);
  int init_games();
  godot::String find_sourcemod_path();
  godot::String get_sourcemod_path();
  void set_sourcemod_path(godot::String path);
  bool isSDKInstalled();
  bool isTF2Installed();
  godot::String get_installed_version(godot::String gameName);
  godot::String get_latest_version(godot::String gameName);
  godot::String is_installed(godot::String gameName);
  int desktop_notification(String title, String desc);

 private:
  void _verify_game(String gameName);
  void _update_game(String gameName);
  void _init_palace();
  palace* p;
};

}  // namespace godot