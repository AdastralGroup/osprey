#include <fremont/fremont.hpp>
#include <sheffield/sheffield.hpp>
#include <vector>


int sheffield::LibTorrentDownload(const std::string &torrentfileurl, const std::string &path) try {
  std::vector<char> torrentfile = fremont().get_bin_data_from_server(torrentfileurl);
  lt::settings_pack p;
  p.set_int(lt::settings_pack::alert_mask, lt::alert_category::status | lt::alert_category::error);
  lt::session ses(p);

  lt::add_torrent_params atp = lt::load_torrent_buffer(torrentfile);
  atp.save_path = path;  // save in current dir
  lt::torrent_handle h = ses.add_torrent(std::move(atp));

  for (;;) {
    std::vector<lt::alert *> alerts;
    ses.pop_alerts(&alerts);

    for (lt::alert const *a : alerts) {
      std::cout << a->message() << std::endl;
      // if we receive the finished alert or an error, we're done
      if (lt::alert_cast<lt::torrent_finished_alert>(a)) {
        goto done;
      }
      if (lt::alert_cast<lt::torrent_error_alert>(a)) {
        goto done;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
done:
  return 0;
} catch (std::exception &e) {
  std::cerr << "Error: " << e.what() << std::endl;
  return 1;
}
