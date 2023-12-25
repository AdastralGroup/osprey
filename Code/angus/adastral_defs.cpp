#include "adastral_defs.h"

void A_printf(const char *const format ,...){
#ifndef GODOT
#warning "GODOT DISABLED."
  va_list argptr;
  va_start(argptr, format);
  vfprintf(stdout, format, argptr);
  va_end(argptr);
#else
//#pragma warning ("GODOT ENABLED.")
  va_list argptr;
  va_start(argptr, format);
  char* str = (char*)malloc(512);
  vsprintf(str, format, argptr);
  godot::UtilityFunctions::print(str);
  va_end(argptr);
  free(str);
#endif
}
