#pragma once

#include <cstdint>
#include <string>
#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/magnet_uri.hpp>

class sheffield {
  std::string m_szAria2cLocation;

 public:
  explicit sheffield(std::string m_szAria2cLocation);
  int AriaDownload(const std::string& szUrl, const std::string& path);
  int LibTorrentDownload(const std::string& magnet, const std::string& path);
};
