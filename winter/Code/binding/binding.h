#include <adastral_defs.h>
#include <sha256.h>
#include <events/progress.hpp>
#include <filesystem>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <net.hpp>
#include <palace.hpp>
#include <thread>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#else
#include <libnotify/notify.h>
#endif

namespace godot
{
class binding : public Node
{
  GDCLASS(binding, Node);

 public:
  binding();
  ~binding();
  void init_palace();
  void _raise_error(std::string error_str, unsigned int err_level);
  void set_sourcemod_path(godot::String path);
  static void _bind_methods();
  int sanity_checks();
  int init_games();
  int update_game(godot::String gameName);
  int verify_game(godot::String gameName);
  int launch_game(String app_id, String arguments);
  int desktop_notification(String title, String desc);
  bool is_app_installed(godot::String app_id);
  godot::Array get_server_games();
  godot::Dictionary get_game_assets(godot::String game_name);
  godot::String find_sourcemod_path();
  godot::String get_sourcemod_path();
  godot::String get_installed_version(godot::String gameName);
  godot::String get_latest_version(godot::String gameName);
  godot::String is_installed(godot::String gameName);

 private:
  void _verify_game(String gameName);
  void _update_game(String gameName);
  void _init_palace();
  palace* p;
};

}  // namespace godot