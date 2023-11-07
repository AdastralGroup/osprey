#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <angus/adastral_defs.h>
namespace godot {

  class sutton : public Node {
    GDCLASS(sutton, Node);

   public:
    sutton();
    ~sutton();
    static void _bind_methods();
    int sanity_checks();
  };
}