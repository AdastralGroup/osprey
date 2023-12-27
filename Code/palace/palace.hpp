/*
 *   Copyright (c) 2023 Copyright The Adastral Group and Contributors

 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.

 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.

 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

// Configure the DLL Import/Export Define

#ifdef BUILDSYSTEM_BUILDING_PALACE_LIB
#ifdef _WIN32
#define AD_PALACE_DLL __declspec(dllexport)
#else
#define AD_PALACE_DLL [[gnu::visibility("default")]]
#endif
#else
#ifdef _WIN32
#define AD_PALACE_DLL __declspec(dllimport)
#else
#define define AD_PALACE_DLL [[gnu::visibility("default")]]
#endif
#endif

#include <curl/curl.h>

#include <kachemak/kachemak.hpp>
#include <nlohmann/json.hpp>


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
  int init_games();
  int update_game(const std::string& gameName);
  int verify_game(const std::string& gameName);
  std::vector<std::string> get_games();
  std::vector<std::string> get_installed_games();
  bool isSDKInstalled();
  bool isTF2Installed();
  nlohmann::json southbankJson;
  std::unordered_map<std::string,GameMetadata*> serverGames;
  std::filesystem::path sourcemodsPath;
  std::filesystem::path find_sourcemod_path();
};