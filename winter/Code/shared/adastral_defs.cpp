#include "adastral_defs.h"

void A_init_error_system() {
  A_error_system = new EventSystem;
}
void A_printf(const char *const format ,...){
#ifndef GODOT
  //#pragma warning ("GODOT DISABLED.")
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

std::string A_SHA256(std::string filename ) {
  std::ifstream file;
  std::istream* input = nullptr;
  file.open(filename, std::ios::in | std::ios::binary);
  SHA256 digestSHA256;
  input = &file;
  char* buffer = new char[144*7*1024];
  while(*input){
    input->read(buffer,144*7*1024);
    auto numBytesRead = size_t(input->gcount());
    digestSHA256.add(buffer,numBytesRead);
  }
  file.close();
  delete[] buffer;
  return digestSHA256.getHash();

}
