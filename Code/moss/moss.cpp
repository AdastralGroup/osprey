#include "moss.hpp"

int moss::ExecWithParam(const std::vector<std::string>& params) {
  std::string param_str;
  for (const auto& i : params) {
    param_str += i + " ";
  }

  printf("%s\n", param_str.c_str());
  return system(param_str.c_str());
}

/*
description:
  recursively delete contents of directory
res:
  0: success
  1: input path doesn't exist
  2: input path isn't a directory
*/
int moss::DeleteDirectoryContent(const std::filesystem::path& dir) {
  if (!std::filesystem::exists(dir)) {
    return 1;
  }
  if (!std::filesystem::is_directory(dir)) {
    return 2;
  }
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (std::filesystem::is_directory(entry)) {
      DeleteDirectoryContent(entry);
      std::filesystem::remove(entry);
    } else {
      std::filesystem::remove(entry);
    }
  }
  return 0;
}

/*
desc:
        extract .zip file
returns:
        0: success
*/

int moss::ExtractZip(const std::string& szInputFile, const std::string& szOutputFile) {
  zip_extract(szInputFile.c_str(), szOutputFile.c_str(), nullptr, nullptr);
  return 0;
}

std::filesystem::path moss::FindSteamPath() { return std::filesystem::path("/lol"); }

bool moss::CheckTF2Installed(const std::filesystem::path& steamDir) {
  std::ifstream file(steamDir / std::filesystem::path("steamapps/libraryfolders.vdf"));
  if (!file.is_open()) {
    return false;
  }
  std::string line;
  while (getline(file, line))
    if (line.find("440") != std::string::npos) {
      std::cout << "TF2 found!" << std::endl;
      return true;
    }
  return false;
}

bool moss::CheckSDKInstalled(const std::filesystem::path& steamDir) {
  std::ifstream file(steamDir / std::filesystem::path("steamapps/libraryfolders.vdf"));
  if (!file.is_open()) {
    return false;
  }
  std::string line;
  while (getline(file, line))
    if (line.find("243750") != std::string::npos) {
      std::cout << "TF2 found!" << std::endl;
      return true;
    }
  return false;
}

void moss::curl_callback(void* buffer, size_t sz, size_t n) { curl_data += (char*)buffer; }

size_t moss::static_curl_callback(void* buffer, size_t sz, size_t n, void* cptr) {
  static_cast<moss*>(cptr)->curl_callback(buffer, sz, n);
  return sz * n;
}

std::string moss::get_string_data_from_server(const std::string& url) {
  CURL* curlHandle = curl_easy_init();
  curl_data = "";
  curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, moss::static_curl_callback);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
  CURLcode res = curl_easy_perform(curlHandle);
  if (res != CURLE_OK) {
    exit(256);
  }
  return curl_data;
}


int moss::sanity_checks() {
  CURL* curlHandle = curl_easy_init();
  curl_easy_setopt(curlHandle, CURLOPT_URL, PRIMARY_URL);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, moss::static_curl_callback);
  curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
  CURLcode res = curl_easy_perform(curlHandle);
  if (res != CURLE_OK) {
    curl_data = "";
    return 1;
  }
  curl_data = "";
  return 0;
}

#if _WIN32
inline bool Is64BitWindows()
{
#if _WIN64
    return true;
#else
    USHORT ProcessMachine;
    USHORT NativeMachine;
    BOOL IsWow64 = IsWow64Process2(GetCurrentProcess(), &ProcessMachine, &NativeMachine);

    if(IsWow64)
    {
        if(NativeMachine == IMAGE_FILE_MACHINE_AMD64) return true;
    }

    return false;
#endif
}
#endif

static std::filesystem::path moss::GetSteamSourcemodPath()
{
#if _WIN32
	char valueData[MAX_PATH];
	DWORD valueLen = MAX_PATH;

	//Check if this is 64 bit or 32 bit process
	const char* subKey = Is64BitWindows() ? "SOFTWARE\\WOW6432Node\\Valve\\Steam" : "\\SOFTWARE\\Valve\\Steam";
	RegGetValueA(HKEY_LOCAL_MACHINE, subKey, "InstallPath", RRF_RT_ANY, nullptr, &valueData, &valueLen);
	return std::filesystem::path(valueData) / "steamapps\\sourcemods";
#else
	//Return normal steam path or use sym link version
	auto normal_steam_linux =  std::filesystem::path("~/.local/share/Steam/steamapps/sourcemods");
	return std::filesystem::exists(normal_steam_linux) ? normal_steam_linux : std::filesystem::path("~/.steam/steam/steamapps/sourcemods");
#endif
}