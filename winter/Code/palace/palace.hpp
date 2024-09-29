#pragma once
#include "vdf_parser.hpp"
#include <adastral_defs.hpp>
#include <cstdlib>
#include <curl/curl.h>
#include <kachemak/kachemak.hpp>
#include <map>
#include <nlohmann/json.hpp>

//! Structure dealing with metadata of the game.
struct GameMetadata
{
    std::string name;
    Kachemak *l1;
};

class palace
{
  public:
    /*!
    Constructor of the class.
    \param path Steam path.
    */
    palace(std::string path = "");
    //! Deconstructor of the class.
    ~palace();

    //! Function to fetch server data.
    void fetch_server_data();

    //! Function to download assets from server to internal Adastral folder.
    void download_assets();

    //! Function to initialize games for Adastral to detect.
    int init_games();

    /*!
        Function to update a specific game.
        \param game_name Name of Adastral-supported game.
    */
    int update_game(const std::string &game_name);

    /*!
    Function to update a specific game in a custom path.
    \param game_name Name of Adastral-supported game.
    \param custom_path Custom path of the Adastral-supported game.
    */
    int update_game_with_path(const std::string &game_name, std::string custom_path);
    /*!
    Function to verify game's integrity.
    \param game_name Name of Adastral-supported game.
    */
    int verify_game(const std::string &game_name);
    /*!
    Function to launch the game itself.
    \param game_name Name of Adastral-supported game.
    \param arguments Launch arguments of the game.
    */
    int launch_game(const std::string &game_name, const std::string &arguments);
    /*!
    Function to check if the app is installed
    \param app_id Steam app id.
    \return If the app is installed or not.
    */
    bool is_app_installed(const std::string &app_id);

    /*!
    Function to get the application path.
    \param app_id Steam app id.
    \return The path of the application.
    */
    std::filesystem::path get_app_path(const std::string &app_id);
    /*!
    Function to get the sourcemod folder path from Steam folder.
    \return Path of the sourcemod folder.*/
    std::filesystem::path find_sourcemod_path();
    /*
    */
    std::filesystem::path get_asset(std::string hash);
    /*!
    Function to get the list of Adastral-supported games.
    \return vector variable containing the list.
    */
    std::vector<std::string> get_games();
    /*!
    Function to get the list of Adastral-supported games that are installed.
    \return vector variable containing the installed games list.
    */
    std::vector<std::string> get_installed_games();

    
    std::filesystem::path sourcemods_path;
    std::filesystem::path steam_path;
    std::unordered_map<std::string, GameMetadata *> server_games;
    std::unordered_map<std::string, std::string> cachemap;
    nlohmann::json southbank_json;
    tyti::vdf::object library_folders;
    tyti::vdf::object config_file;
};