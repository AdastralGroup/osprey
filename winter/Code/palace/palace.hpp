#pragma once
#include <curl/curl.h>

#include <kachemak/kachemak.hpp>
#include <nlohmann/json.hpp>
#include <adastral_defs.h>
#include <cstdlib>
#include <map>
#include "vdf_parser.hpp"
#include "adastral_defs.h"

struct GameMetadata{
  std::string name;
  Kachemak* l1;
};

class palace {
public:
    palace();
    ~palace();
    void fetch_server_data();
    void download_assets();
    int init_games();
    int update_game(const std::string& gameName);
    int update_game_with_path(const std::string& gameName, std::string customPath);
    int verify_game(const std::string& gameName);
    int launch_game(const std::string& game_name, const std::string& arguments);
    std::vector<std::string> get_games();
    std::vector<std::string> get_installed_games();
    bool is_app_installed(const std::string& app_id);
    std::filesystem::path get_app_path(const std::string& app_id);
    nlohmann::json southbankJson;
    std::unordered_map<std::string,GameMetadata*> serverGames;
    std::unordered_map<std::string,std::string> cachemap;
    std::filesystem::path get_asset(std::string hash);
    std::filesystem::path sourcemodsPath;
    std::filesystem::path find_sourcemod_path();
    tyti::vdf::object library_folders;
    tyti::vdf::object config_file;
};