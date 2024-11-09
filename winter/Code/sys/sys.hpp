#pragma once
#include <adastral_defs.hpp>
#include <filesystem>
#include <string>
#include <vdf_parser.hpp>
#include <vector>
#include "zip/zip.h"

#ifdef _WIN32
#include <Windows.h>
#include <Winreg.h>
#endif

class sys
{
 public:
    /*!
        Function to execute a system function through a parameter.
        \param params Set of parameters.
    */
  static int exec_with_param(const std::vector<std::string>& params);
   /*!
     Function to recursively delete contents of directory.
     \param dir path of the directory
     \return 0 if success, 1 if input path doesn't exist, 2 if input path isn't a directory.
 */
  static int delete_directory_content(const std::filesystem::path& dir);
  /*!
    Function to extract zip files.
    \param input_file Input of the zip archive file.
    \param output_file Direction of the output zip archive file.
  */
  static int extract_zip(const std::string& input_file, const std::string& output_file);

  /*!
    Function to get path of Steam.
    \return Steam path.
  */
  static std::filesystem::path get_steam_path();

  /*!
    Function to parse vdf files.
    \param file_path File path of the vdf file.
    \return Result of parsed VDF file.
  */
  static tyti::vdf::object parse_vdf_file(std::filesystem::path file_path);
};
