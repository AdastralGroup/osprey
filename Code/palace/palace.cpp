#include "palace.h"

palace::palace() { sanity_checks(); }

int palace::sanity_checks() {
  CURL* curlHandle = curl_easy_init();
  curl_easy_setopt(curlHandle, CURLOPT_URL, PRIMARY_URL);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, palace::static_curl_callback);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
  CURLcode res = curl_easy_perform(curlHandle);
  if (res != CURLE_OK) {
    curl_data = "";
    return 1;
  }
  curl_data = "";
  return 0;
}

size_t palace::static_curl_callback(void* buffer, size_t sz, size_t n, void* f) {
  static_cast<palace*>(f)->curl_callback(buffer, sz, n);
  return sz * n;
}

void palace::get_server_games() { get_southbank_from_server("https://adastral.net/a/southbank.json"); }
void palace::get_southbank_from_server(const std::string& url) {
  CURL* curlHandle = curl_easy_init();
  curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, palace::static_curl_callback);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
  CURLcode res = curl_easy_perform(curlHandle);
  if (res != CURLE_OK) {
    exit(256);
  }
  southbankJson = nlohmann::json::parse(curl_data);  // do error checking here
  std::cout << southbankJson["sb_ver"] << std::endl;
}
void palace::curl_callback(void* buffer, size_t sz, size_t n) { curl_data += (char*)buffer; }
