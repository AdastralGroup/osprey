#pragma once
#include <curl/curl.h>
#include <adastral_defs.h>

#include <events/eventsystem.hpp>
#include <events/progress.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


#define PRIMARY_URL "https://fusion.adastral.net/"
class net {
 public:
  static std::string download_to_temp(std::string url, std::string name, bool progress = false,EventSystem* event = nullptr, std::filesystem::path* path = nullptr);
  std::string get_string_data_from_server(const std::string& url);
  std::vector<char> get_bin_data_from_server(const std::string& url);
  //Get path object for the systems sourcemods folder (win/linux)
 private:
  static int progress_func(void* ptr, curl_off_t TotalToDownload, curl_off_t NowDownloaded, curl_off_t TotalToUpload,
                           curl_off_t NowUploaded);
  static size_t static_curl_callback(void *buffer, size_t sz, size_t n, void *cptr);
  void curl_callback(void *buffer, size_t n); // callback for the below. string only. we could r
  std::string curl_string_data; // we do curl calls synchronously, it's usually all string data so that ends up in here.
  std::vector<char> curl_bin_data; //idk lol
  bool bin;
};
