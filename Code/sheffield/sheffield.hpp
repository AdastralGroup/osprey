#pragma once

#include <string>
#include <cstdint>

class sheffield {
    static std::string m_szAria2cLocation;
public:
    sheffield(const std::string m_szAria2cLocation);
    int AriaDownload(const std::string& szUrl, const std::string& path);
};
