#pragma once

#include <events/eventsystem.hpp>
#include <filesystem>
#include <string>

class Version {
 public:
  Version(const std::filesystem::path& szInstallPath, const std::filesystem::path& szDataDirectory,
          const std::string& szSourceUrl = "https://wiki.tf2classic.com/kachemak/");
  const std::string& GetInstalledVersion();
  void Verify();
  virtual int Install() = 0;

 protected:
  void FindInstalledVersion();

  std::string m_szInstalledVersion;
  std::filesystem::path m_szInstallPath;
  std::filesystem::path m_szDataDirectory;
  std::string m_szSourceUrl;
  EventSystem m_eventSystem;
};