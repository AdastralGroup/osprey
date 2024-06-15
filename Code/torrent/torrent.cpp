
#include <torrent.hpp>

namespace {

using clk = std::chrono::steady_clock;

// return the name of a torrent status enum
char const* state(lt::torrent_status::state_t s)
{
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
  switch(s) {
    case lt::torrent_status::checking_files: return "checking";
    case lt::torrent_status::downloading_metadata: return "dl metadata";
    case lt::torrent_status::downloading: return "downloading";
    case lt::torrent_status::finished: return "finished";
    case lt::torrent_status::seeding: return "seeding";
    case lt::torrent_status::checking_resume_data: return "checking resume";
    default: return "<>";
  }
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

} // anonymous namespace

//Adding in extra printf functions to log actions when possible.
int torrent::LibTorrentDownload(const std::string &torrentfileurl, const std::string &path, EventSystem* event) try {
  //First, list what are the current parameters.
  A_printf("[torrent] Current torrent file URL: %s", torrentfileurl.c_str());
  A_printf("[torrent] Current path the file is heading to: %s", path.c_str());

  //Then check the torrent file. Since it is a vector of char, it should bring in
  //like weird garbled text to show that there's something being fetched from.
  std::vector<char> torrentfile = net().get_bin_data_from_server(torrentfileurl);
  
  A_printf("[torrent] the torrent file should be loaded.");
  
  lt::settings_pack p;
  p.set_int(lt::settings_pack::alert_mask, lt::alert_category::status | lt::alert_category::error);
  lt::session ses(p);

  A_printf("[torrent] Settings pack should be initialized.");

  lt::add_torrent_params atp = lt::load_torrent_buffer(torrentfile);
  atp.save_path = path;  // save in current dir
  A_printf("[torrent] Current atp.save_path: %s", atp.save_path.c_str());
  lt::torrent_handle h = ses.add_torrent(std::move(atp));
  for (;;) {
    std::vector<lt::alert *> alerts;
    ses.pop_alerts(&alerts);

    for (lt::alert const *a : alerts) {
      A_printf(a->message().c_str());
      // if we receive the finished alert or an error, we're done
      if (lt::alert_cast<lt::torrent_finished_alert>(a)) {
        goto done;
      }
      if (lt::alert_cast<lt::torrent_error_alert>(a)) {
        goto done;
      }

      if (auto st = lt::alert_cast<lt::state_update_alert>(a)) {
        if (st->status.empty()) continue;
        // we only have a single torrent, so we know which one the status is for
        lt::torrent_status const& s = st->status[0];
        if(event != nullptr) { // probably stupid to do this check on every event
          ProgressUpdateMessage message(s.download_payload_rate, s.progress);
          event->TriggerEvent(message);
        }
        //std::cout << '\r' << state(s.state) << ' '
        //  << (s.download_payload_rate / 1000) << " kB/s "
        //  << (s.total_done / 1000) << " kB ("
        //  << (s.progress_ppm / 10000) << "%) downloaded ("
        //  << s.num_peers << " peers)\x1b[K";
        //std::cout.flush();
      }
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ses.post_torrent_updates();
  }
done:
  return 0;
} catch (std::exception &e) {
  A_error(e.what());
  A_printf("[torrent] Error: %s\n",e.what());
  return 1;
}
