#pragma once
#include <adastral_defs.hpp>
#include <curl/curl.h>
#include <events/eventsystem.hpp>
#include <events/progress.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define SB_URL   "https://cockatoo.kate.pet/api/v3/Southbank"
#define ROOT_URL "https://fusion.adastral.net/"
class net
{
 public:
    /*!
        Function to download items to a temporary folder.
        \param url URL link to download from.
        \param name Name of the temp folder.
        \param progress State of the progress
        \param event Kind of event enabled for downloading, defaults to nullptr.
        \param path Path of the temporary folder, defaults to nullptr.
    */
  static std::string download_to_temp(std::string url, std::string name, bool progress = false,
                                      EventSystem* event = nullptr, std::filesystem::path* path = nullptr);

  /*!
    Function to get string data from server.
    \param url URL link to download from.
  */
  std::string get_string_data_from_server(const std::string& url);
  /*!
  Function to get string data from server.
  \param url URL link to download from.
*/
  std::vector<char> get_bin_data_from_server(const std::string& url);
  // Get path object for the systems sourcemods folder (win/linux)
 private:
  static int progress_func(void* ptr, curl_off_t total_to_download, curl_off_t now_downloaded, curl_off_t total_to_upload,
                           curl_off_t now_uploaded);
  static size_t static_curl_callback(void* buffer, size_t sz, size_t n, void* cptr);
  void curl_callback(void* buffer, size_t n);
  std::string curl_string_data;  // we do curl calls synchronously, it's usually all string data so that ends up in here.
  std::vector<char> curl_bin_data;  // idk lol
  bool bin;
};
