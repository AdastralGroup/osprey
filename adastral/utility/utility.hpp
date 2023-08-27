#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

class Utility
{
    public:
    static int ExecWithParam(const std::vector<std::string>& params);
    static int DeleteDirectoryContent(const std::filesystem::path& dir);
};
