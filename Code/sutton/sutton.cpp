#include "sutton.h"
  sutton::sutton() {
    auto ad = adastral::ADProjectRegister();
    ad.RegisterClass<sutton>("adastral",*this);
  }