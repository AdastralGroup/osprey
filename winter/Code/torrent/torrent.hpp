#pragma once

#include <cstdint>
#include <events/event.hpp>
#include <events/eventsystem.hpp>
#include <events/progress.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/load_torrent.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_handle.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <net.hpp>
#include <string>
#include <vector>

class torrent
{
  public:
    /*!
    Function to download from a turrent file using libtorrent.
    \param torrent_url URL of the torrent.
    \param path Path to save the contents to.
    \param event Events from the EventSystem.
    */
    static int libtorrent_download(const std::string &torrent_url, const std::string &path, EventSystem *event = nullptr);
};
