#pragma once

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fremont/fremont.hpp>
#include <optional>
#include <string>
#include <system_error>
#include <version/version.hpp>

struct KachemakVersion {
  std::string szFileName;
  std::string szDownloadUrl;
  std::size_t lDownloadSize;
  std::size_t lExtractSize;
  std::string szVersion;
  std::string szSignature;
};

struct KachemakPatch {
  std::string szUrl;
  std::string szFilename;
  std::size_t lTempRequired;
};

enum class FreeSpaceCheckCategory { Temporary, Permanent };

class Kachemak : public Version {
 public:
  Kachemak(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
           const std::string& szSourceUrl, const std::string installed_version);
  std::optional<KachemakVersion> GetVersion(const std::string& version);
  std::optional<KachemakPatch> GetPatch(const std::string& version);
  std::optional<KachemakVersion> GetLatestVersion();
  int FreeSpaceCheck(const uintmax_t size, const FreeSpaceCheckCategory& category);
  int PrepareSymlink();
  int DoSymlink();
  int Update();
  virtual int Install();
  int Extract(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize);
  int ButlerVerify(const std::string& szSignature, const std::string& szGameDir, const std::string& szRemote);
  int ButlerPatch(const std::string& sz_url, const std::filesystem::path& sz_stagingDir,
                  const std::string& sz_patchFileName, const std::string& sz_gameDir, const uintmax_t downloadSize);

 private:
  int ButlerParseCommand(const std::string& command);

 private:
  nlohmann::ordered_json m_parsedVersion;
  std::filesystem::path m_szTempPath;

  std::filesystem::path m_szButlerLocation;
  std::filesystem::path m_szAria2cLocation;

  inline static const char* TO_SYMLINK[][2] = {
      {"bin/server.so", "bin/server_srv.so"},
  };
};