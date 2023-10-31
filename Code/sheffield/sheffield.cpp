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

int sheffield::LibTorrentDownload(const std::string &magnet, const std::string &path) try{
    lt::settings_pack p;
    p.set_int(lt::settings_pack::alert_mask, lt::alert_category::status
                                             | lt::alert_category::error);
    lt::session ses(p);

    lt::add_torrent_params atp = lt::parse_magnet_uri(magnet);
    atp.save_path = path; // save in current dir
    lt::torrent_handle h = ses.add_torrent(std::move(atp));

    for (;;) {
        std::vector<lt::alert *> alerts;
        ses.pop_alerts(&alerts);

        for (lt::alert const *a: alerts) {
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
}
catch (std::exception& e)
{
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
