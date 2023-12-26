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


void A_printf(const char *const format ,...);
#endif