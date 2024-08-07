#pragma once
#include <KeyValue.h>
#include <adastral_defs.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "zip/zip.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winreg.h>
#endif


class sys {
 public:
  static int ExecWithParam(const std::vector<std::string>& params);
  static int DeleteDirectoryContent(const std::filesystem::path& dir);
  static int ExtractZip(const std::string& szInputFile, const std::string& szOutputFile);
  static KeyValueRoot* ParseVDFFile(std::filesystem::path file_path);
  static std::filesystem::path GetSteamPath();
};
