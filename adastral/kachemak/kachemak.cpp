#include <kachemak/kachemak.hpp>

Kachemak::Kachemak(
	const std::filesystem::path& szInstallPath,
	const std::filesystem::path& szDataDirectory,
	const std::string& szSourceUrl)
	: Version(szInstallPath, szDataDirectory, szSourceUrl)
{
	// placeholder
	m_parsedVersion = R"({
		"versions":
		{
			"200": {"signature": "tf2classic200.sig", "heal" : "tf2classic-2.0.0-heal.zip"},
			"201" : {},
			"202" : {},
			"203" : {"signature": "tf2classic203.sig", "heal" : "tf2classic-2.0.3-heal.zip"},
			"204" : {"signature": "tf2classic204.sig", "heal" : "tf2classic-2.0.4-heal.zip"},
			"210" : {"url": "tf2classic-2.1.0.meta4", "file" : "tf2classic-2.1.0.tar.zst", "presz" : 5050680575, "postsz" : 13884901888, "signature" : "tf2classic210.sig", "heal" : "tf2classic-2.1.0-heal.zip"},
			"211" : {"url": "tf2classic-2.1.1.meta4", "file" : "tf2classic-2.1.1.tar.zst", "presz" : 5050680575, "postsz" : 13884901888, "signature" : "tf2classic211.sig", "heal" : "tf2classic-2.1.1-heal.zip"}
		},
		"patches":
		{
		"204": {"url": "tf2classic-patch-204-211.pwr", "file" : "tf2classic-patch-204-211.pwr", "tempreq" : 12050680575},
			"210" : {"url": "tf2classic-patch-210-211.meta4", "file" : "tf2classic-patch-210-211.pwr", "tempreq" : 7500000000}
		}
	})"_json;

	m_szTempPath = std::filesystem::temp_directory_path().string();

}

KachemakVersion Kachemak::GetVersion(const std::string& version)
{
	if (!m_parsedVersion["versions"][version].is_object()) {
		// handle error;	
	}

	nlohmann::json jsonVersion = m_parsedVersion["versions"][version];
	KachemakVersion ret = {
		.szFileName = jsonVersion["file"].get<std::string>(),
		.szDownloadUrl = jsonVersion["url"].get<std::string>(),
		.lDownloadSize = jsonVersion["presz"].get<std::size_t>(),
		.lExtractSize = jsonVersion["postsz"].get<std::size_t>(),
		.szHealUrl = jsonVersion["heal"].get<std::string>(),
		.szVersion = version,
		.szSignature = jsonVersion["signature"].get<std::string>(),
	};

	return ret;
}

KachemakVersion Kachemak::GetLatestVersion()
{
	std::string versionId;
	for (auto& el : m_parsedVersion["versions"].items()) { versionId = el.key(); };

	return GetVersion(versionId);
}

/*
description:
  wrapper for butler cli to verify installation
res:
  0: success
  i: return code from butler (when not 0)
*/
int Kachemak::ButlerVerify(
	const std::string& szSignature,
	const std::string& szGameDir,
	const std::string& szRemote)
{
	std::stringstream params;

	// {m_butlerLoation} verify {szSignature} {szGameDir} --heal=archive{szRemote}
	params << m_szButlerLocation.c_str()
	<< " verify "
	<< szSignature.c_str() << " "
	<< szGameDir.c_str() << " "
	<< "--heal=archive" << szRemote;


	int res = system(params.str().c_str());
	if (res != 0)
	{
		std::cerr << "Failed to verify with butler: " << res << std::endl;
		return res;
	}

	return 0;
}
// TODO - 
int Kachemak::PrepareSymlink()
{
	return 0;
}

/*
description:
  check free space for specific category provided.
res:
  0: success
  1: not enough temp storage
  2: not enough permanent storage
*/
int Kachemak::FreeSpaceCheck(
	const uintmax_t size,
	const FreeSpaceCheckCategory& category)
{
	switch(category)
	{
		case FreeSpaceCheckCategory::Temporary:
			if (std::filesystem::space(m_szTempPath).free < size)
			{
				return 1;
			}
			break;
		case FreeSpaceCheckCategory::Permanent:
			if (std::filesystem::space(m_szInstallPath).free < size)
			{
				return 2;
			}
			break;
	}
	return 0;
}

/*
description:
  patch installation
res:
  0: success
  1: failed to download patch
  2: failed to apply patch*/
int Kachemak::ButlerPatch(
	const std::string& sz_url,
	const std::filesystem::path& sz_stagingDir,
	const std::string& sz_patchFileName,
	const std::string& sz_gameDir)
{
	bool stagingDir_exists = std::filesystem::exists(sz_stagingDir);
	bool stagingDir_isDir = std::filesystem::is_directory(sz_stagingDir);
	if (!stagingDir_exists)
	{
		if (!std::filesystem::create_directory(sz_stagingDir))
		{
			std::cerr << "[ButlerPatch] Failed to create directory: " << sz_stagingDir.string() << std::endl;
		}
	}
	if (stagingDir_exists && stagingDir_isDir)
	{
		switch(Utility::DeleteDirectoryContent(sz_stagingDir))
		{
			case 1:
				std::cerr << "[ButlerPatch] Failed to delete staging directory content (doesn't exist)";
				break;
			case 2:
				std::cerr << "[ButlerPatch] Failed to delete staging directory content (not a directory)";
				break;
		}
	}

	#pragma region Download Patch
	std::vector<std::string> dl_params = 
	{
		m_szAria2cLocation,
		"--max-connection-per-server=16",
		"-UAdastral-master",
		"--disable-ipv6=true",
		"--allow-piece-length-change=true",
		"--max-concurrent-downloads=16",
		"--optimize-concurrent-downloads=true",
		"--check-certificate=false",
		"--check-integrity=true",
		"--auto-file-renaming=false",
		"--continue=true",
		"--allow-overwrite=true",
		"--console-log-level=error",
		"--summary-interval=0",
		"--bt-hash-check-seed=false",
		"--seed-time=0",
		"-d",
		m_szTempPath.string(),
		sz_url
	};
	std::cout << "[ButlerPatch] Downloading Patch" << std::endl;
	int dl_res = Utility::ExecWithParam(dl_params);
	if (dl_res != 0)
	{
		std::cerr << "[ButlerPatch] Failed to download patch: " << dl_res << std::endl;
		return 1;
	}
	#pragma endregion

	std::filesystem::path tempPath = m_szTempPath / sz_patchFileName;

	std::vector<std::string> apply_params =
	{
		m_szButlerLocation,
		"apply",
		"--staging-dir" + sz_stagingDir.string(),
		tempPath,
		sz_gameDir
	};
	std::cout << "[ButlerPatch] Applying patch" << std::endl;
	int apply_res = Utility::ExecWithParam(apply_params);
	if (apply_res != 0)
	{
		std::cerr << "[ButlerPatch] Failed to apply patch: "<< apply_res << std::endl;
		return 2;
	}


	switch(Utility::DeleteDirectoryContent(sz_stagingDir))
	{
		case 1:
			std::cerr << "[ButlerPatch] Failed to delete staging directory content (doesn't exist)";
			break;
		case 2:
			std::cerr << "[ButlerPatch] Failed to delete staging directory content (not a directory)";
			break;
	}
	return 0;
}