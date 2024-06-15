#include "net.hpp"


void net::curl_callback(void* buffer, size_t n) {
  if(!bin)
    curl_string_data += (char*)buffer;
  else{
    curl_bin_data.insert(curl_bin_data.end(),(char*)buffer,(char*)buffer+n);
  }
}

size_t net::static_curl_callback(void* buffer, size_t sz, size_t n, void* cptr) {
  static_cast<net*>(cptr)->curl_callback(buffer, n);
  return sz*n;
}

size_t cool_curl_callback(char *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

std::string net::get_string_data_from_server(const std::string& url) {
  CURL* curlHandle = curl_easy_init();
  std::string curl_string_data_local;
  curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, cool_curl_callback);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &curl_string_data_local);
  CURLcode res = curl_easy_perform(curlHandle);
  if (res != CURLE_OK) {
    exit(256);
  }
  curl_easy_cleanup(curlHandle);
  curl_string_data = curl_string_data_local;
  return curl_string_data;
}

int net::progress_func(void* ptr, curl_off_t TotalToDownload, curl_off_t NowDownloaded, curl_off_t TotalToUpload,
                           curl_off_t NowUploaded) {
  if(NowDownloaded != 0 && TotalToDownload != 0) {
    long double percentage = (static_cast<long double>(NowDownloaded)/static_cast<long double>(TotalToDownload));
    if(ptr != nullptr) {
      ProgressUpdateMessage message(0, percentage);
      auto* events = (EventSystem*)ptr;
      events->TriggerEvent(message);
    }
    //A_printf("[net] Downloading... %Lg%%, now downloaded %ld, total %ld\n",percentage,NowDownloaded,TotalToDownload);
  }
  return 0;
}

std::string net::download_to_temp(std::string url, std::string name, bool progress,EventSystem* event, std::filesystem::path* path){
  std::string temp_path;
  if(path != nullptr) {
    temp_path = (*path / std::filesystem::path(name)).string();
  }else {
    temp_path = (std::filesystem::temp_directory_path() / std::filesystem::path(name)).string();
  }
    auto fp = fopen(temp_path.c_str(),"wb");
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  if(progress) {
    if(event != nullptr) {
      A_printf("[net] events should have been enabled... \n");
      curl_easy_setopt(curl, CURLOPT_XFERINFODATA, event);
    }
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_func);
  }
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
    std::filesystem::permissions(temp_path,std::filesystem::perms::all);
    return temp_path;
}



std::vector<char> net::get_bin_data_from_server(const std::string& url) {
  bin = true;
  CURL* curlHandle = curl_easy_init();
  curl_bin_data.clear();
  curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, net::static_curl_callback);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
  CURLcode res = curl_easy_perform(curlHandle);
  if (res != CURLE_OK) {
        exit(256);
  }
  curl_easy_cleanup(curlHandle);
  return curl_bin_data;
}


