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

#include <sha256.h>

#include <events/eventsystem.hpp>
#include <events/error.hpp>
#include <fstream>
#include <map>
#include <memory>
#include <string>


inline EventSystem* A_error_system;
enum error_level { INFO, WARNING, SERIOUS, PANIC };
inline std::map<error_level, std::string> error_string_map{
  {INFO,"info"},
  {WARNING,"warning"},
  {SERIOUS,"serious"},
  {PANIC,"panic"}
};
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define popen _popen
#define BUTLER "butler.exe"
#define pclose _pclose
#else
#define BUTLER "butler"
#endif
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args );
std::string A_SHA256(std::string filename);
void A_printf(const char *const format ,...);
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
  int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
  if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
  auto size = static_cast<size_t>( size_s );
  std::unique_ptr<char[]> buf( new char[ size ] );
  std::snprintf( buf.get(), size, format.c_str(), args ... );
  return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}
template<typename ... Args>
void A_error(const std::string& format, Args ... args) {
  std::string error_detail = string_format(format,args ...);
  A_printf("### ERROR: %s\n",error_detail.c_str());
#ifndef GODOT
#else
  ErrorMessage e = ErrorMessage(std::move(error_detail));
  A_error_system->TriggerEvent(e);
#endif
}
void A_init_error_system();

