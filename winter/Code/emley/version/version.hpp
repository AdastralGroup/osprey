#pragma once

#include <events/eventsystem.hpp>
#include <filesystem>
#include <string>

class Version
{
 public:
  Version(const std::filesystem::path& game_path, const std::filesystem::path& folder_name,
          const std::string& source_url);
  virtual int verify() = 0;
  virtual int install(std::filesystem::path path) = 0;
  const std::string& get_installed_version_code();
  std::string name;
  EventSystem event_system;  // we need to yoink it into palace and then sutton

 protected:
  std::string installed_version_code;
  std::string source_url;
  std::filesystem::path game_path;
  std::filesystem::path folder_name;
};