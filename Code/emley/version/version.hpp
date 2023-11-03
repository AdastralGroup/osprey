#pragma once

#include <events/eventsystem.hpp>
#include <filesystem>
#include <string>

class Version {
 public:
  Version(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
          const std::string& szSourceUrl);
  const std::string& GetInstalledVersion();
  std::string name;
  virtual int Verify() = 0;
  virtual int Install() = 0;

 protected:

  std::string m_szInstalledVersion;
  std::filesystem::path m_szSourcemodPath;
  std::filesystem::path m_szFolderName;
  std::string m_szSourceUrl;
  EventSystem m_eventSystem;
};