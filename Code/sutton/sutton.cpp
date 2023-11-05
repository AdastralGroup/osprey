#include "sutton.h"

void sayHello() { std::cout << "Hello, World!" << std::endl; }
  sutton::sutton() {
    auto ad = adastral::ADProjectRegister();
    ad.RegisterClass<sutton>("adastral",*this);
    std::function<void()> atest;
    atest = sayHello;
    ad.BindGodotMethod<sutton>(atest, (godot::String&)"sayHello");
  }

  void sutton::_bind_methods() {}
