#pragma once

#include <string>
#include <filesystem>

class Version {
public:
	Version(const std::filesystem::path& szInstallPath, const std::filesystem::path& szDataDirectory, const std::string& szSourceUrl = "https://wiki.tf2classic.com/kachemak/");
	const std::string& GetInstalledVersion();
	void Verify();
protected:
	void FindInstalledVersion();
	
	std::string m_szInstalledVersion;
	std::filesystem::path m_szInstallPath;
	std::filesystem::path m_szDataDirectory;
	std::string m_szSourceUrl;
};	