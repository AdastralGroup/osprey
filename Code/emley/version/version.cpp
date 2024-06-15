#include <fstream>
#include <version/version.hpp>

Version::Version(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
                 const std::string& szSourceUrl)
    : m_szSourcemodPath(szSourcemodPath), m_szFolderName(szFolderName), m_szSourceUrl(szSourceUrl) {
}

const std::string& Version::GetInstalledVersion() { return m_szInstalledVersion; }