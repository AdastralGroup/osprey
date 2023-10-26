#pragma once
#include <zip/zip.h>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#define PRIMARY_URL "https://adastral.net/"
class moss {
 public:
  static int ExecWithParam(const std::vector<std::string>& params);
  static int DeleteDirectoryContent(const std::filesystem::path& dir);
  static int ExtractZip(const std::string& szInputFile, const std::string& szOutputFile);
  static std::filesystem::path FindSteamPath();
  static bool CheckTF2Installed(const std::filesystem::path& steamDir);
  static bool CheckSDKInstalled(const std::filesystem::path& steamDir);
  int sanity_checks();
  std::string get_string_data_from_server(const std::string& url);
 private:
  static size_t static_curl_callback(void *buffer, size_t sz, size_t n, void *cptr);
  void curl_callback(void *buffer, size_t sz, size_t n); // callback for the below. string only. we could r
  std::string curl_data; // we do curl calls synchronously, it's usually all string data so that ends up in here.
};
