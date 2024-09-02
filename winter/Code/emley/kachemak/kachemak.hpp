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
  std::string file_name;
  std::string download_url;
  std::size_t download_size;
  std::size_t extract_size;
  std::string version;
  std::string signature;
};

struct KachemakPatch
{
  std::string url;
  std::string filename;
  std::size_t temp_required;
};

enum class FreeSpaceCheckCategory
{
  Temporary,
  Permanent
};

class Kachemak : public Version
{
 public:
  Kachemak(const std::filesystem::path& sourcemod_path, const std::filesystem::path& folder_name,
           const std::string& source_url, const std::filesystem::path& butler_path);
  std::string get_installed_version_tag();
  std::string get_latest_version();
  std::string get_latest_version_tag();
  int free_space_check(const uintmax_t size, const FreeSpaceCheckCategory& category);
  int free_space_check_path(const uintmax_t size, const std::filesystem::path custom_path);
  int prepare_symlink();
  int do_symlink();
  int do_symlink_path(std::filesystem::path custom_path);
  int update();
  virtual int install();
  virtual int install_path(std::filesystem::path custom_path);
  int verify();
  int extract(const std::string& input_file, const std::string& output_directory, const size_t& size);
  int extract_path(const std::string& input_file, const std::string& output_directory, const size_t& size);
  bool M_force_verify = false;

 private:
  int butler_parse_command(const std::string& command);
  std::optional<KachemakVersion> get_km_version(const std::string& version);
  std::optional<KachemakPatch> get_patch(const std::string& version);
  std::optional<KachemakVersion> get_latest_km_version();
  int butler_verify(const std::string& signature, const std::string& game_dir, const std::string& remote);
  int butler_patch(const std::string& url, const std::filesystem::path& staging_dir,
                  const std::string& patch_file_name, const std::string& game_dir, const uintmax_t download_size);
  void find_installed_version();
  nlohmann::ordered_json M_parsed_version;
  std::filesystem::path M_temp_path;
  std::filesystem::path M_butler_location;
  inline static const char* TO_SYMLINK[][2] = {
    { "bin/server.so", "bin/server_srv.so" },
  };
  void write_version();
};