#include <kachemak/kachemak.hpp>
#include <events/progress.hpp>
#include <events/error.hpp>

#if !defined(popen)
	#define popen _popen
	#define pclose _pclose
#endif

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
			"200": {"signature": "tf2classic200.sig", "heal": "tf2classic-2.0.0-heal.zip"},
			"201": {},
			"202": {},
			"203": {"signature": "tf2classic203.sig", "heal": "tf2classic-2.0.3-heal.zip"},
			"204": {"signature": "tf2classic204.sig", "heal": "tf2classic-2.0.4-heal.zip"},
			"210": {"url": "tf2classic-2.1.0.meta4", "file": "tf2classic-2.1.0.tar.zst", "presz": 5050680575, "postsz": 13884901888, "signature": "tf2classic210.sig", "heal": "tf2classic-2.1.0-heal.zip"},
			"211": {"url": "tf2classic-2.1.1.meta4", "file": "tf2classic-2.1.1.tar.zst", "presz": 5050680575, "postsz": 13884901888, "signature": "tf2classic211.sig", "heal": "tf2classic-2.1.1-heal.zip"},
			"212": {"url": "tf2classic-2.1.2.meta4", "file": "tf2classic-2.1.2.tar.zst", "presz": 5050680575, "postsz": 13884901888, "signature": "tf2classic212.sig", "heal": "tf2classic-2.1.2-heal.zip"}
		},
		"patches":
		{
			"204": {"url": "tf2classic-patch-204-212.pwr", "file": "tf2classic-patch-204-212.pwr", "tempreq": 12050680575},
			"210": {"url": "tf2classic-patch-210-212.pwr", "file": "tf2classic-patch-210-212.pwr", "tempreq": 7500000000},
			"211": {"url": "tf2classic-patch-211-212.meta4", "file": "tf2classic-patch-211-212.pwr", "tempreq": 12050680575}
		}
	})"_json;

	m_szTempPath = std::filesystem::temp_directory_path().string();

	m_szButlerLocation = std::filesystem::current_path() / "bin" / "butler";
	m_szAria2cLocation = std::filesystem::current_path() / "bin" / "aria2c";


	m_eventSystem.RegisterListener(EventType::kOnUpdate, [](Event& ev) {
		printf("Progress: %f\n", ((ProgressUpdateMessage&)ev).GetProgress());
	});
	
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

int Kachemak::PrepareSymlink()
{
#ifdef _WIN32
	return 0;
#else
	for (const auto& item : TO_SYMLINK) {
		const std::filesystem::path symlinkPath = m_szInstallPath / item[1];
		if (std::filesystem::exists(symlinkPath) && !std::filesystem::is_symlink(symlinkPath)) {
			std::filesystem::remove(symlinkPath);
		}
	}

	return 0;
#endif
}

int Kachemak::DoSymlink()
{
#ifdef _WIN32
	return 0;
#else
	for (const auto& item : TO_SYMLINK) {
		const std::filesystem::path symlinkPath = m_szInstallPath / m_szDataDirectory / item[1];
		if (!std::filesystem::exists(symlinkPath)) {
			std::filesystem::create_symlink(m_szInstallPath / item[0], symlinkPath);
		}
	}

	return 0;
#endif
}
/*
description:
  routine for updating whatever is installed
res:
  0: success
  1: symlink fail
  2: space check fail
*/
int Kachemak::Update()
{
	int symlinkRes = PrepareSymlink();
	if (symlinkRes != 0)
	{
		return 1;
	}

	std::string local_version = "204";

	nlohmann::json patch_json = m_parsedVersion["patches"];
	std::string patch_url = patch_json[local_version]["url"].get<std::string>();
	std::string patch_file = patch_json[local_version]["file"].get<std::string>();
	uintptr_t patch_tempreq = patch_json[local_version]["tempreq"].get<uintptr_t>();

	if (FreeSpaceCheck(patch_tempreq, FreeSpaceCheckCategory::Permanent) != 0) {
		return 2;
	}

	nlohmann::json version_json = m_parsedVersion["versions"];
	std::string signature_url = version_json[local_version]["signature"].get<std::string>();
	std::string heal_url = version_json[local_version]["heal"].get<std::string>();

	// full signature url
	std::stringstream sigUrlFull_ss;
	sigUrlFull_ss << m_szSourceUrl << signature_url;
	// Data path for current install
	std::filesystem::path dataDir_path = m_szInstallPath / m_szDataDirectory;
	std::stringstream healUrl_ss;
	healUrl_ss << m_szSourceUrl << heal_url;
	int verifyRes = ButlerVerify(
		sigUrlFull_ss.str(),
		dataDir_path.string(),
		healUrl_ss.str());

	std::stringstream patchUrlFull_ss;
	patchUrlFull_ss << m_szSourceUrl << patch_url;
	std::filesystem::path stagingPath = m_szInstallPath / "butler-staging";
	int patchRes = ButlerPatch(
		patchUrlFull_ss.str(),
		stagingPath.string(),
		patch_file,
		dataDir_path.string());

	DoSymlink();
	return 0;
}


/*
desc:
	extract .tar.zstd file to directory
res:
	0: success
	1: not enough free space
	2: failed to delete tmp file
	11: failed to open input file
	12: failed to create zstd context
	13: failed to allocate memory
	14: failed to decompress
	21: failed to open tar file
*/
int Kachemak::Extract(const std::string& szInputFile, const std::string& szOutputDirectory, const size_t& szSize)
{
	if (FreeSpaceCheck(szSize * 2, FreeSpaceCheckCategory::Permanent) != 0)
	{
		return 1;
	}



	std::FILE* tmpf = std::tmpfile();
	std::string tmpf_loc = std::to_string(fileno(tmpf));
	int zstd_res = Utility::ExtractZStd(szInputFile, tmpf_loc);
	if (zstd_res > 0)
		return zstd_res + 10;
	int tar_res = Utility::ExtractTar(tmpf_loc, szOutputDirectory);
	if (tar_res > 0)
		return tar_res + 20;
	int remove_res = remove(tmpf_loc.c_str());
	if (!remove_res)
	{
		std::cerr << "[Kachemak::Extract] failed to delete tmp file: " << remove_res << std::endl;
		return 2;
	}

	return 0;
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
	params << m_szButlerLocation.string()
	<< " verify "
	<< szSignature.c_str() << " "
	<< "\"" << szGameDir.c_str() << "\"" << " "
	<< "--heal=archive," << szRemote
	<< " --json";

	int status = ButlerParseCommand(params.str());
	if (status != NULL)
		return status;

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
		m_szAria2cLocation.string(),
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

	std::stringstream params;

	params << m_szButlerLocation.string() << " ";
	params << "apply" << " ";
	params << "--staging-dir=\"" << sz_stagingDir.string() << "\"" << " ";
	params << "\"" << tempPath.string() << "\"" << " ";
	params << "\"" << sz_gameDir << "\"" << " ";
	params << "--json";

	std::cout << "[ButlerPatch] Applying patch" << std::endl;
	int butlerStatus = ButlerParseCommand(params.str());
	if (butlerStatus != NULL) {
		std::cerr << "[ButlerPatch] Failed to apply patch: " << butlerStatus << std::endl;
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

int Kachemak::ButlerParseCommand(const std::string& command)
{
	FILE* pipe = popen(command.c_str(), "rb");
	if (!pipe) {
		printf("Failed to create pipe for butler verification\n");
		return 1;
	}

	const uint16_t bufSize = 4096;
	char buffer[bufSize];
	while (fgets(buffer, bufSize, pipe)) {
		nlohmann::json jsonBuffer = nlohmann::json::parse(buffer, nullptr, false);
		if (jsonBuffer.is_discarded())
			continue;

		if (jsonBuffer.contains("type")) {
			std::string messageType = jsonBuffer["type"].get<std::string>();

			if (messageType.compare("progress") == NULL) {
				ProgressUpdateMessage message(jsonBuffer["bps"].get<float>(), jsonBuffer["progress"].get<float>());
				m_eventSystem.TriggerEvent(message);
			}
			else if (messageType.compare("error") == NULL) {
				ErrorMessage message(jsonBuffer["message"].get<std::string>());
				m_eventSystem.TriggerEvent(message);
			}
		}
	}

	pclose(pipe);

	return 0;
}
