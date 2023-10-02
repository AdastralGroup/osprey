#pragma once

#include <utility/utility.hpp>
#include <version/version.hpp>
#include <json/single_include/nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <cstdlib>
#include <system_error>
#include <array>
#include <filesystem>
#include <cstdio>

struct KachemakVersion {
	std::string szFileName;
	std::string szDownloadUrl;
	std::size_t lDownloadSize;
	std::size_t lExtractSize;
	std::string szHealUrl;
	std::string szVersion;
	std::string szSignature;
};

enum class FreeSpaceCheckCategory
{
	Temporary,
	Permanent
};

class Kachemak : public Version {
public:

	Kachemak(const std::filesystem::path& szInstallPath, const std::filesystem::path& szDataDirectory, const std::string& szSourceUrl = "https://wiki.tf2classic.com/kachemak/");
	KachemakVersion GetVersion(const std::string& version);
	KachemakVersion GetLatestVersion();
	int FreeSpaceCheck(
		const uintmax_t size,
		const FreeSpaceCheckCategory& category);
	int PrepareSymlink();
	int DoSymlink();
	int Update();
	int Extract(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize);
	int ButlerVerify(
		const std::string& szSignature,
		const std::string& szGameDir,
		const std::string& szRemote);
	int ButlerPatch(
		const std::string& sz_url,
		const std::filesystem::path& sz_stagingDir,
		const std::string& sz_patchFileName,
		const std::string& sz_gameDir);

private:
	int ButlerParseCommand(const std::string& command);
private:
	nlohmann::json m_parsedVersion;
	std::filesystem::path m_szTempPath;

	std::filesystem::path m_szButlerLocation;
	std::filesystem::path m_szAria2cLocation;

	inline static const char* TO_SYMLINK[][2] = {
		{"bin/server.so", "bin/server_srv.so"},
	};

};