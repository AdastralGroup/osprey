#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/godot.hpp>
class sutton: public godot::Node { GDCLASS(sutton, godot::Node);
 public:
  sutton();
  static void _bind_methods();
};