#ifndef WINTER_SHARED_HEADER
#define WINTER_SHARED_HEADER
#ifndef GODOT
#include <cstdio>
#include <cstdarg>
#else
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#endif
#include <string>
#include <fstream>
#include <sha256.h>
std::string A_SHA256(std::string filename);
void A_printf(const char *const format ,...);
#endif