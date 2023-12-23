#include <angus/adastral_defs.h>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/json.hpp>
#include <palace/palace.hpp>
#include <thread>
namespace godot {

  class sutton : public Node {
    GDCLASS(sutton, Node);

   public:
    sutton();
    ~sutton();
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


   private:
    void _verify_game(String gameName);
    void _init_palace();
    palace* p;
  };


}