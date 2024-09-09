#pragma once

#define WIN32_LEAN_AND_MEAN
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <sys.hpp>
#include <version/version.hpp>
#include <torrent.hpp>

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
  Kachemak(const std::filesystem::path& game_path, const std::filesystem::path& folder_name,
           const std::string& source_url, const std::filesystem::path& butler_path);
  virtual int install(std::filesystem::path path);
  int free_space_check(uintmax_t size, const FreeSpaceCheckCategory &category, std::string perm_path = "");
  virtual int update();
  virtual int verify();
  int extract(const std::string& input_file, const std::string& output_directory, const size_t& size);
  std::string get_installed_version_tag();
  std::string get_latest_version_code();
  std::string get_latest_version_tag();
  bool force_verify = false;

 private:
  void find_installed_version();
  void write_version();
  int butler_parse_command(const std::string& command);
  int butler_verify(const std::string& signature, const std::string& game_dir, const std::string& remote);
  int butler_patch(const std::string& url, const std::filesystem::path& staging_dir,
                  const std::string& patch_file_name, const std::string& game_dir, const uintmax_t download_size);
  std::optional<KachemakVersion> get_km_version(const std::string& version);
  std::optional<KachemakPatch> get_patch(const std::string& version);
  std::optional<KachemakVersion> get_latest_km_version();
  std::filesystem::path temp_path;
  std::filesystem::path butler_location;
  nlohmann::ordered_json parsed_version;
};