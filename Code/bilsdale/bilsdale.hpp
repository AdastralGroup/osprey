#pragma once

#include <cstdint>
#include <string>
#include <angus/adastral_defs.h>
#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/load_torrent.hpp>

class bilsdale {

 public:
  static int LibTorrentDownload(const std::string& torrent_url, const std::string& path);
};
