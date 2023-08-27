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
