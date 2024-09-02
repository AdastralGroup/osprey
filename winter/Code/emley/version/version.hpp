#pragma once

#include <events/eventsystem.hpp>
#include <filesystem>
#include <string>

class Version
{
 public:
  Version(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
          const std::string& szSourceUrl);
  const std::string& get_installed_version();
  std::string name;
  virtual int verify() = 0;
  virtual int install() = 0;
  EventSystem m_eventSystem;  // we need to yoink it into palace and then sutton

 protected:
  std::string m_szInstalledVersion;
  std::filesystem::path m_szSourcemodPath;
  std::filesystem::path m_szFolderName;
  std::string m_szSourceUrl;
};