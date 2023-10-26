#pragma once

#include <cstdint>
#include <string>

class sheffield {
  std::string m_szAria2cLocation;

 public:
  explicit sheffield(std::string m_szAria2cLocation);
  int AriaDownload(const std::string& szUrl, const std::string& path);
};
