#pragma once
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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define popen _popen
#define BUTLER "butler.exe"
#define pclose _pclose
#else
#define BUTLER "butler"
#endif

std::string A_SHA256(std::string filename);
void A_printf(const char *const format ,...);