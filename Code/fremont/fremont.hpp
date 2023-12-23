#pragma once
#include "zip/zip.h"
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <angus/adastral_defs.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#else
#include <libnotify/notify.h>
#endif

#define PRIMARY_URL "https://fusion.adastral.net/"
class fremont {
 public:
  static int ExecWithParam(const std::vector<std::string>& params);
  static int DeleteDirectoryContent(const std::filesystem::path& dir);
  static int ExtractZip(const std::string& szInputFile, const std::string& szOutputFile);
  static bool CheckTF2Installed(const std::filesystem::path& steamDir);
  static bool CheckSDKInstalled(const std::filesystem::path& steamDir);
  static std::string download_to_temp(std::string url, std::string name);
  int sanity_checks();
  std::string get_string_data_from_server(const std::string& url);
  std::vector<char> get_bin_data_from_server(const std::string& url);
  //Get path object for the systems sourcemods folder (win/linux)
  static std::filesystem::path GetSteamPath();
  static std::string get_butler();
  static int DesktopNotif(const std::string title,std::string desc);
 private:
  static size_t static_curl_callback(void *buffer, size_t sz, size_t n, void *cptr);
  void curl_callback(void *buffer, size_t n); // callback for the below. string only. we could r
  std::string curl_string_data; // we do curl calls synchronously, it's usually all string data so that ends up in here.
  std::vector<char> curl_bin_data; //idk lol
  bool bin;
};
