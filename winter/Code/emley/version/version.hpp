#pragma once

#include <events/eventsystem.hpp>
#include <filesystem>
#include <string>

class Version
{
 public:
  Version(const std::filesystem::path& sourcemod_path, const std::filesystem::path& folder_name,
          const std::string& source_url);
  virtual int verify() = 0;
  virtual int install() = 0;
  const std::string& get_installed_version();
  std::string name;
  EventSystem M_event_system;  // we need to yoink it into palace and then sutton

 protected:
  std::string M_installed_version;
  std::string M_source_url;
  std::filesystem::path M_sourcemod_path;
  std::filesystem::path M_folder_name;
};