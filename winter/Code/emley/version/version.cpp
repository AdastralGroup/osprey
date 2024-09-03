#include "version.hpp"

Version::Version(const std::filesystem::path& sourcemod_path, const std::filesystem::path& folder_name,
                 const std::string& source_url) :
    M_sourcemod_path(sourcemod_path),
    M_folder_name(folder_name),
    M_source_url(source_url)
{
}

const std::string& Version::get_installed_version() { return M_installed_version; }