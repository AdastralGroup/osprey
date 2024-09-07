#include "version.hpp"

Version::Version(const std::filesystem::path &sourcemod_path, const std::filesystem::path &folder_name, const std::string &source_url)
    : sourcemod_path(sourcemod_path), folder_name(folder_name), source_url(source_url)
{
}

const std::string &Version::get_installed_version_code()
{
    return installed_version_code;
}