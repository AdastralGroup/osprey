#pragma once
#include "vdf_parser.hpp"
#include <adastral_defs.hpp>
#include <cstdlib>
#include <curl/curl.h>
#include <kachemak/kachemak.hpp>
#include <map>
#include <nlohmann/json.hpp>

struct GameMetadata
{
    std::string name;
    Kachemak *l1;
};

class palace
{
  public:
    palace(std::string path = "");
    ~palace();
    void fetch_server_data();
    void download_assets();
    int init_games();
    int update_game(const std::string &game_name);
    int update_game_with_path(const std::string &game_name, std::string custom_path);
    int verify_game(const std::string &game_name);
    int launch_game(const std::string &game_name, const std::string &arguments);
    bool is_app_installed(const std::string &app_id);
    std::filesystem::path get_app_path(const std::string &app_id);
    std::filesystem::path find_sourcemod_path();
    std::filesystem::path get_asset(std::string hash);
    std::vector<std::string> get_games();
    std::vector<std::string> get_installed_games();
    std::filesystem::path sourcemods_path;
    std::filesystem::path steam_path;
    std::unordered_map<std::string, GameMetadata *> server_games;
    std::unordered_map<std::string, std::string> cachemap;
    nlohmann::json southbank_json;
    tyti::vdf::object library_folders;
    tyti::vdf::object config_file;
};