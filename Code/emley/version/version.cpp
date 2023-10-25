#include <fstream>
#include <version/version.hpp>

Version::Version(const std::filesystem::path& szInstallPath, const std::filesystem::path& szDataDirectory,
                 const std::string& szSourceUrl)
    : m_szInstallPath(szInstallPath), m_szDataDirectory(szDataDirectory), m_szSourceUrl(szSourceUrl) {
  FindInstalledVersion();
}

const std::string& Version::GetInstalledVersion() { return m_szInstalledVersion; }

void Version::FindInstalledVersion() {
  std::ifstream version(m_szInstallPath / m_szDataDirectory / ".adastral");
  if (!version.is_open()) {
    // handle error;
  }

  std::stringstream content;
  content << version.rdbuf();
  m_szInstalledVersion = content.str();
}

void Version::Verify() {
  // verification process through butler
}