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

#include <coldfield/External/ADRegisterProject.h>
#include <curl/curl.h>

#include <emley/kachemak/kachemak.hpp>
#include <moss/json/single_include/nlohmann/json.hpp>

class palace {
 public:
  palace();

  void get_server_games();
  int init_games();
  int update_game(const std::string& gameName);

  nlohmann::json southbankJson;
  std::unordered_map<std::string,Kachemak*> serverGames;
};