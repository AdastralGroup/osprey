#include <fstream>
#include <version/version.hpp>

Version::Version(const std::filesystem::path& szSourcemodPath, const std::filesystem::path& szFolderName,
                 const std::string& szSourceUrl)
    : m_szSourcemodPath(szSourcemodPath), m_szFolderName(szFolderName), m_szSourceUrl(szSourceUrl) {
  FindInstalledVersion();
}

const std::string& Version::GetInstalledVersion() { return m_szInstalledVersion; }

void Version::FindInstalledVersion() {
  //std::ifstream version(m_szSourcemodPath / m_szFolderName / ".adastral");
  //if (!version.is_open()) {
  //  // handle error;
  //}
  //
  //std::stringstream content;
  //content << version.rdbuf();
  //m_szInstalledVersion = content.str();
}

void Version::Verify() {
  // verification process through butler
}