#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include "emley/zip/zip.h"

class moss
{
    public:
    static int ExecWithParam(const std::vector<std::string>& params);
    static int DeleteDirectoryContent(const std::filesystem::path& dir);
    static int ExtractZip(const std::string& szInputFile, const std::string& szOutputFile);
};
