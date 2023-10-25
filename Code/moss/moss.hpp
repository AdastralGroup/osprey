#pragma once
#include <zip/zip.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class moss {
 public:
  static int ExecWithParam(const std::vector<std::string>& params);
  static int DeleteDirectoryContent(const std::filesystem::path& dir);
  static int ExtractZip(const std::string& szInputFile, const std::string& szOutputFile);
  static std::filesystem::path FindSteamPath();
  static bool CheckTF2Installed(const std::filesystem::path& steamDir);
  static bool CheckSDKInstalled(const std::filesystem::path& steamDir);
};
