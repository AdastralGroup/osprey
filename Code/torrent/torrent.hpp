#pragma once

#include <cstdint>
#include <string>
#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/session_handle.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/load_torrent.hpp>
#include <net.hpp>

class torrent {

 public:
  static int LibTorrentDownload(const std::string& torrent_url, const std::string& path);
};
