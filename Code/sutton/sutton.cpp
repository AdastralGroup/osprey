#include "sutton.h"

using namespace godot;

void sutton::_bind_methods() {
  ClassDB::bind_method(D_METHOD("sanity_checks"),&sutton::sanity_checks);
}

  sutton::sutton(){
    UtilityFunctions::print("nice");
  }


  int sutton::sanity_checks(){
    UtilityFunctions::print("sutton: works!");
    A_printf("aprintf test... %d %d\n",42,8);
    UtilityFunctions::print("print test");
    A_printf("aprintf test 2..\n");
    return 1;
  }
  sutton::~sutton(){
    UtilityFunctions::print("bye bye");
  }





