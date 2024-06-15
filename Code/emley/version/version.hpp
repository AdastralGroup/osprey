#pragma once


#include <filesystem>
#include <string>
#include <events/eventsystem.hpp>

class Version {
 public:
  Version(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
          const std::string& szSourceUrl);
  const std::string& GetInstalledVersion();
  std::string name;
  virtual int Verify() = 0;
  virtual int Install() = 0;
  EventSystem m_eventSystem; // we need to yoink it into palace and then sutton

 protected:

  std::string m_szInstalledVersion;
  std::filesystem::path m_szSourcemodPath;
  std::filesystem::path m_szFolderName;
  std::string m_szSourceUrl;
};