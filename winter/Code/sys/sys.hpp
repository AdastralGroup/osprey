#pragma once
#include <adastral_defs.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vdf_parser.hpp>
#include <vector>
#include "zip/zip.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winreg.h>
#endif

class sys
{
 public:
  static int exec_with_param(const std::vector<std::string>& params);
  static int delete_directory_content(const std::filesystem::path& dir);
  static int extract_zip(const std::string& input_file, const std::string& output_file);
  static std::filesystem::path get_steam_path();
  static tyti::vdf::object parse_vdf_file(std::filesystem::path file_path);
};
