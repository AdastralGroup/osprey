#pragma once

#include <cstdint>
#include <string>
#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/load_torrent.hpp>

class sheffield {

 public:
  explicit sheffield(std::string m_szAria2cLocation);
  static int LibTorrentDownload(const std::string& torrent_url, const std::string& path);
};
