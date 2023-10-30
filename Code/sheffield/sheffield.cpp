#include <fremont/fremont.hpp>
#include <sheffield/sheffield.hpp>
#include <vector>

sheffield::sheffield(const std::string vm_szAria2cLocation) { m_szAria2cLocation = vm_szAria2cLocation; }

int sheffield::AriaDownload(const std::string& szUrl, const std::string& path) {
  std::vector<std::string> params = {m_szAria2cLocation,
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
                                     "--human-readable=false",
                                     "-d",
                                     path,
                                     szUrl};

  printf("Downloading %s\n", szUrl.c_str());
  int status = fremont::ExecWithParam(params);
  if (status != 0) {
    printf("Download failed, Status: %d\n", status);
    return 1;
  }

  return 0;
}