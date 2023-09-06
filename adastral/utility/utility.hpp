#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include <zstd.h>
#include <archive.h>
#include <archive_entry.h>


class Utility
{
    public:
    static int ExecWithParam(const std::vector<std::string>& params);
    static int DeleteDirectoryContent(const std::filesystem::path& dir);
    static int ExtractZStd(const std::string& szInputFile, const std::string& szOutputDirectory);
    static int ExtractTar(const std::string& szInputFile, const std::filesystem::path& szOutputDirectory);
};
