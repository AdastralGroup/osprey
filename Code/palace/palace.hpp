#pragma once
#include <curl/curl.h>

#include <kachemak/kachemak.hpp>
#include <nlohmann/json.hpp>
#include <adastral_defs.h>

struct GameMetadata{
  std::string name;
  Kachemak* l1;
};


class palace {
 public:
  palace();
  ~palace();
  bool TF2Installed = false;
  bool SDKInstalled = false;
  void fetch_server_data();
  void download_assets();
  int init_games();
  int update_game(const std::string& gameName);
  int verify_game(const std::string& gameName);
  std::vector<std::string> get_games();
  std::vector<std::string> get_installed_games();
  bool isSDKInstalled();
  bool isTF2Installed();
  nlohmann::json southbankJson;
  std::unordered_map<std::string,GameMetadata*> serverGames;
  std::unordered_map<std::string,std::string> cachemap;
  std::filesystem::path get_asset(std::string hash);
  std::filesystem::path sourcemodsPath;
  std::filesystem::path find_sourcemod_path();
};