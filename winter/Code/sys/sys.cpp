#include "sys.hpp"


int sys::ExecWithParam(const std::vector<std::string>& params) {
  std::string param_str;
  for (const auto& i : params) {
    param_str += i + " ";
  }

  A_printf("%s\n", param_str.c_str());
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
int sys::DeleteDirectoryContent(const std::filesystem::path& dir) {
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

int sys::ExtractZip(const std::string& szInputFile, const std::string& szOutputFile) {
  A_printf("[sys/Extract] Extracting %s to %s..\n",szInputFile.c_str(), szOutputFile.c_str());
  int ret = zip_extract(szInputFile.c_str(), szOutputFile.c_str(), nullptr, nullptr);
  return ret;
}

bool sys::CheckTF2Installed(const std::filesystem::path& steamDir) {
  std::ifstream file(steamDir / std::filesystem::path("steamapps/libraryfolders.vdf"));
  if (!file.is_open()) {
    return false;
  }
  std::string line;
  while (getline(file, line))
    if (line.find("440") != std::string::npos) {
      A_printf("[sys] TF2 found!\n");
      return true;
    }
  return false;
}

bool sys::CheckSDKInstalled(const std::filesystem::path& steamDir) {
  std::ifstream file(steamDir / std::filesystem::path("steamapps/libraryfolders.vdf"));
  if (!file.is_open()) {
    return false;
  }
  std::string line;
  while (getline(file, line))
    if (line.find("243750") != std::string::npos) {
      A_printf("[sys] SDK2013MP found!\n");
      return true;
    }
  return false;
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

std::filesystem::path sys::GetSteamPath()
{
#if _WIN32
	char valueData[MAX_PATH];
	DWORD valueLen = MAX_PATH;

	//Check if this is 64 bit or 32 bit process
	const char* subKey = Is64BitWindows() ? "SOFTWARE\\WOW6432Node\\Valve\\Steam" : "\\SOFTWARE\\Valve\\Steam";

    //Get steam install dir from registry
	RegGetValueA(HKEY_LOCAL_MACHINE, subKey, "InstallPath", RRF_RT_ANY, nullptr, &valueData, &valueLen);
    if (valueData[0] == 0)
    {
        //Registry key did not exist/had no value
        return std::filesystem::path();
    }

	return std::filesystem::path(valueData);
#else
    std::string home = getenv("HOME");
    auto path = std::filesystem::path(home + "/.local/share/Steam/");
    if(std::filesystem::exists(path)){
        return std::filesystem::canonical(path);
    }else{
        return std::filesystem::path("");
    }
#endif
}


