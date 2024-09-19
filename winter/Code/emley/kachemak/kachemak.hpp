#pragma once

#define WIN32_LEAN_AND_MEAN
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <sys.hpp>
#include <version/version.hpp>
#include <torrent.hpp>

//! Structure for data files that deals with Kachemak.
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

//! Enum class that specifies the category of the directory to check if it has free space.
enum class FreeSpaceCheckCategory
{
  Temporary,
  Permanent
};

//! The Kachemak class.
class Kachemak : public Version
{
 public:
    //! Constructor of the class.
   /*!
    \param sourcemod_path The path of the sourcemod directory, found within Steam directory.
    \param folder_name The name of the folder for the game.
    \param source_url The link to the game's downloadable path online.
    \param butler_path The path of butler.
   */
  Kachemak(const std::filesystem::path& sourcemod_path, const std::filesystem::path& folder_name,
           const std::string& source_url, const std::filesystem::path& butler_path);

  //! Function to install sourcemod games.
  /*!
    \return Status code for the installation.
  */
  virtual int install();
  //! Function to install sourcemod games through a specific path.
  /*!
  * \param custom_path Path where the game would be installed.
    \return Status code post-installation.
  */
  virtual int install_path(std::filesystem::path custom_path);

  //! Function to check on free spaces at specific directories, depending on its category.
  /*!
    \param size Specified Size to check on.
    \param category The category of the directory.
  */
  int free_space_check(const uintmax_t size, const FreeSpaceCheckCategory& category);
  //! Function to check on free spaces at specific directories, depending on its path.
  /*!
  * \param size Specified Size to check on.
  * \param category The path of the directory.
  * 
  * Status code post-checking are as such:
  * 0: success
  * 1: not enough temp storage
  * 2: not enough permanent storage
  * \return Status code post-checking.
  */
  int free_space_check_path(const uintmax_t size, const std::filesystem::path custom_path);
  //! Function to update games installed by Adastral
  /*!
  * Status code post-update are as such:
  * 0: success
  * 1: symlink fail
  * 2: space check fail
  * 3: already on latest / verification needed first
  * 
  * \return Status code post-update.
  */
  int update();

  //! Function to verify games installed by Adastral
  /*!
  * Status code post-verification are as such:
  * 0: success
  * 4: not installed
  * \return Status code post-verification.
  */ 
  int verify();
  //! Function to extract games stored as .zip files.
  /*!
  * \param input_file Name of the input file.
  * \param output_directory Name of the output directory.
  * \param size Size of the file.
  * Status code post-extract are as such:
  * -1: failure
  * 0: success
  * 1: not enough free space
  * \return Status code post-extract.
  */
  int extract(const std::string& input_file, const std::string& output_directory, const size_t& size);
  //! Function to extract games stored as .zip files to a custom path.
  /*!
   * \param input_file Name of the input file.
   * \param output_directory Path of the output directory.
   * \param size Size of the file.
   * Status code post-extract are as such:
   * -1: failure
   * 0: success
   * 1: not enough free space
   * \return Status code post-extract.
   */
  int extract_path(const std::string& input_file, const std::string& output_directory, const size_t& size);
  //! Function to get the tag of an installed game.
  std::string get_installed_version_tag();
  //! Function to get the latest version code of an installed game.
  std::string get_latest_version_code();
  //! Function to get the latest version tag of an installed game.
  std::string get_latest_version_tag();
  //! Boolean that checks whether verification is forced or not.
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