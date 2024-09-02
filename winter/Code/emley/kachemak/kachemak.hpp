#pragma once

#include <array>
#include <cstdio>
#include <cstdlib>
#include <events/error.hpp>
#include <filesystem>
#include <iostream>
#include <net.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <sys.hpp>
#include <system_error>
#include <torrent.hpp>
#include <version/version.hpp>

struct KachemakVersion
{
  std::string szFileName;
  std::string szDownloadUrl;
  std::size_t lDownloadSize;
  std::size_t lExtractSize;
  std::string szVersion;
  std::string szSignature;
};

struct KachemakPatch
{
  std::string szUrl;
  std::string szFilename;
  std::size_t lTempRequired;
};

enum class FreeSpaceCheckCategory
{
  Temporary,
  Permanent
};

class Kachemak : public Version
{
 public:
  Kachemak(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
           const std::string& szSourceUrl, const std::filesystem::path& ButlerPath);
  std::string get_installed_version_tag();
  std::string get_latest_version();
  std::string get_latest_version_tag();
  bool force_verify = false;
  int free_space_check(const uintmax_t size, const FreeSpaceCheckCategory& category);
  int free_space_check_path(const uintmax_t size, const std::filesystem::path customPath);
  int prepare_symlink();
  int do_symlink();
  int do_symlink_path(std::filesystem::path customPath);
  int update();
  virtual int install();
  virtual int install_path(std::filesystem::path customPath);
  int verify();
  int extract(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize);
  int extract_path(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize);

 private:
  int butler_parse_command(const std::string& command);

 private:
  std::optional<KachemakVersion> get_km_version(const std::string& version);
  std::optional<KachemakPatch> get_patch(const std::string& version);
  std::optional<KachemakVersion> get_latest_km_version();
  int butler_verify(const std::string& szSignature, const std::string& szGameDir, const std::string& szRemote);
  int butler_patch(const std::string& sz_url, const std::filesystem::path& sz_stagingDir,
                  const std::string& sz_patchFileName, const std::string& sz_gameDir, const uintmax_t downloadSize);
  void find_installed_version();
  nlohmann::ordered_json m_parsedVersion;
  std::filesystem::path m_szTempPath;

  std::filesystem::path m_szButlerLocation;

  inline static const char* TO_SYMLINK[][2] = {
    { "bin/server.so", "bin/server_srv.so" },
  };
  void write_version();
};