#include "sutton.h"

#include <string>

using namespace godot;

void sutton::_bind_methods() {
  ADD_SIGNAL(
      MethodInfo("game_verified", PropertyInfo(Variant::STRING, "status"), PropertyInfo(Variant::STRING, "game")));
  ADD_SIGNAL(
      MethodInfo("game_updated", PropertyInfo(Variant::STRING, "status"), PropertyInfo(Variant::STRING, "game")));
  ADD_SIGNAL(MethodInfo("progress_update",PropertyInfo(Variant::FLOAT,"progress"),PropertyInfo(Variant::STRING,"game")));
  ADD_SIGNAL(MethodInfo("palace_started"));
  ClassDB::bind_method(D_METHOD("desktop_notification"), &sutton::desktop_notification);
  ClassDB::bind_method(D_METHOD("init_palace"), &sutton::init_palace);
  ClassDB::bind_method(D_METHOD("sanity_checks"), &sutton::sanity_checks);
  ClassDB::bind_method(D_METHOD("get_game_assets"), &sutton::get_game_assets);
  ClassDB::bind_method(D_METHOD("update_game"), &sutton::update_game);
  ClassDB::bind_method(D_METHOD("verify_game"), &sutton::verify_game);
  ClassDB::bind_method(D_METHOD("isSDKInstalled"), &sutton::isSDKInstalled);
  ClassDB::bind_method(D_METHOD("isTF2Installed"), &sutton::isTF2Installed);
  ClassDB::bind_method(D_METHOD("init_games"), &sutton::init_games);
  ClassDB::bind_method(D_METHOD("get_server_games"), &sutton::get_server_games);
  ClassDB::bind_method(D_METHOD("find_sourcemod_path"), &sutton::find_sourcemod_path);
  ClassDB::bind_method(D_METHOD("get_sourcemod_path"), &sutton::get_sourcemod_path);
  ClassDB::bind_method(D_METHOD("set_sourcemod_path"), &sutton::set_sourcemod_path);
  ClassDB::bind_method(D_METHOD("get_installed_version"), &sutton::get_installed_version);
  ClassDB::bind_method(D_METHOD("get_latest_version"), &sutton::get_latest_version);
}

sutton::sutton() {
  UtilityFunctions::print("[sutton] Initialised. Ready to fire up palace.");
}


Dictionary sutton::get_game_assets(String game_name) {
  Dictionary dict;
  dict["id"] = game_name;
  std::string str_gamename = game_name.utf8().get_data();
  int num = 0;
  for (auto i : p->southbankJson["games"].items()) {
    if (i.key() != str_gamename) {
      continue;
    }
    for (auto x : i.value()["belmont"].items()) {
      std::string val = x.value();
      if (val[0] != '#') {
        std::string file_name = str_gamename + std::to_string(num) + ".png";
        std::string path = fremont::download_to_temp(val, file_name);
        A_printf("%s\n", path.c_str());
        dict[(godot::String)x.key().c_str()] = path.c_str();
        num++;
      } else {
        dict[(godot::String)x.key().c_str()] = (String)val.c_str();
      }
    }
  }
  return dict;
}

int sutton::sanity_checks() { return 1; }

void sutton::init_palace() { // yucky hack but we can manually start palace from godot now - means we don't hang at the
  // godot splash
  std::thread thread_obj(&sutton::_init_palace,this);
  thread_obj.detach();
}

void sutton::_init_palace() {
  UtilityFunctions::print("[sutton] Firing up palace!");
  p = new palace;
  emit_signal("palace_started");
}

// WRAPPERS!
int sutton::update_game(godot::String gameName) {
  std::thread thread_obj(&sutton::_update_game,this,gameName);
  thread_obj.detach();
  return 0;
}

void sutton::_update_game(String gameName) {
  int z =  p->update_game(gameName.utf8().get_data());
  emit_signal("game_updated", String(std::to_string(z).c_str()), gameName);
}


int sutton::verify_game(godot::String gameName) {
  //
  std::thread thread_obj(&sutton::_verify_game,this,gameName);
  thread_obj.detach();
  return 0;
}


void sutton::_verify_game(String gameName) {
  int z =  p->verify_game(gameName.utf8().get_data());
  emit_signal("game_verified", String(std::to_string(z).c_str()), gameName);
}

bool sutton::isSDKInstalled() {
  return p->isSDKInstalled();
};
bool sutton::isTF2Installed() { return p->isTF2Installed(); };

int sutton::init_games() {
  int ret = p->init_games();
  for(auto i : p->serverGames) {
    i.second->l1->m_eventSystem.RegisterListener(EventType::kOnUpdate,[this,i](Event& ev) {
    double prog = ((ProgressUpdateMessage&)ev).GetProgress();
        emit_signal("progress_update",i.first.c_str(),prog);
    });
  }
  return ret;
}


godot::String sutton::find_sourcemod_path() {
  auto path = p->find_sourcemod_path();
  return path.c_str();
};
godot::String sutton::get_sourcemod_path() { return p->sourcemodsPath.c_str(); };
// practically a wrapper
godot::Array sutton::get_server_games() {
  std::vector<std::string> games = p->get_games();
  godot::Array gd_games;
  for (const auto& i : games) {
    gd_games.push_back(i.c_str());
  }
  return gd_games;
}
godot::String sutton::get_installed_version(godot::String gameName) {
  if (p->serverGames.count(gameName.utf8().get_data()) == 0) {
    return "";
  }
  return p->serverGames[gameName.utf8().get_data()]->l1->GetInstalledVersion().c_str();
};
godot::String sutton::get_latest_version(godot::String gameName) {
  if (p->serverGames.count(gameName.utf8().get_data()) == 0) {
    return "";
  }
  return p->serverGames[gameName.utf8().get_data()]->l1->GetLatestVersion().c_str();
}
void sutton::set_sourcemod_path(godot::String gd_path) {
  std::filesystem::path path = std::filesystem::path(gd_path.utf8().get_data());
  if (exists(path)) {
    p->sourcemodsPath = path;
  }
};

int sutton::desktop_notification(String title, String desc) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  return 0;
#else
  notify_init("Adastral");
  NotifyNotification* notification = notify_notification_new(title.ascii().get_data(), desc.ascii().get_data(), nullptr);
  notify_notification_set_timeout(notification, 3000);
  notify_notification_show(notification, nullptr);
  g_object_unref(G_OBJECT(notification));
  return 0;
#endif
}



sutton::~sutton() { UtilityFunctions::print("bye bye"); }