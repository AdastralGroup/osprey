#pragma once
#include <adastral_defs.h>
#include <curl/curl.h>
#include <cstdlib>
#include <kachemak/kachemak.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include "adastral_defs.h"
#include "vdf_parser.hpp"

struct GameMetadata
{
  std::string name;
  Kachemak* l1;
};

class palace
{
 public:
  palace(std::string path = "");
  ~palace();
  void fetch_server_data();
  void download_assets();
  int init_games();
  int update_game(const std::string& game_name);
  int update_game_with_path(const std::string& game_name, std::string custom_path);
  int verify_game(const std::string& game_name);
  int launch_game(const std::string& game_name, const std::string& arguments);
  std::vector<std::string> get_games();
  std::vector<std::string> get_installed_games();
  bool is_app_installed(const std::string& app_id);
  std::filesystem::path get_app_path(const std::string& app_id);
  nlohmann::json M_southbank_json;
  std::unordered_map<std::string, GameMetadata*> M_server_games;
  std::unordered_map<std::string, std::string> M_cachemap;
  std::filesystem::path get_asset(std::string hash);
  std::filesystem::path M_sourcemods_path;
  std::filesystem::path M_steam_path;
  std::filesystem::path find_sourcemod_path();
  tyti::vdf::object M_library_folders;
  tyti::vdf::object M_config_file;
};