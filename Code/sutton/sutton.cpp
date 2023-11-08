#include "sutton.h"

using namespace godot;

void sutton::_bind_methods() {
  ClassDB::bind_method(D_METHOD("sanity_checks"),&sutton::sanity_checks);
  ClassDB::bind_method(D_METHOD("get_game_assets"),&sutton::get_game_assets);
  ClassDB::bind_method(D_METHOD("update_game"),&sutton::update_game);
  ClassDB::bind_method(D_METHOD("verify_game"),&sutton::verify_game);
  ClassDB::bind_method(D_METHOD("isSDKInstalled"),&sutton::isSDKInstalled);
  ClassDB::bind_method(D_METHOD("isTF2Installed"),&sutton::isTF2Installed);
  ClassDB::bind_method(D_METHOD("init_games"),&sutton::init_games);
  ClassDB::bind_method(D_METHOD("get_server_games"),&sutton::get_server_games);
  ClassDB::bind_method(D_METHOD("find_sourcemod_path"),&sutton::find_sourcemod_path);
  ClassDB::bind_method(D_METHOD("get_sourcemod_path"),&sutton::get_sourcemod_path);
  ClassDB::bind_method(D_METHOD("set_sourcemod_path"),&sutton::set_sourcemod_path);
}

  sutton::sutton(){
    UtilityFunctions::print("[sutton] Initialised. Firing up palace!");
    p = new palace;
  }


  int sutton::sanity_checks(){
    return 1;
  }

  godot::Dictionary sutton::get_game_assets(godot::String game_name){
    godot::Dictionary dict;
    dict["id"] = game_name;
    std::string str_gamename = game_name.utf8().get_data();
    int num = 0;
    for(auto i: p->southbankJson["games"].items()){
      if(i.key() != str_gamename){continue;}
      for(auto x: i.value()["belmont"].items()){
        std::string val = x.value();
        if(val[0] != '#'){
          std::string file_name = str_gamename + std::to_string(num) + ".png";
          std::string path = fremont().download_to_temp(val,file_name);
          A_printf("%s\n",path.c_str());
          dict[(godot::String)x.key().c_str()] = (godot::String)path.c_str();
          num++;
        }else{
          dict[(godot::String)x.key().c_str()] = (godot::String)val.c_str();
        }
      }
    }
    return dict;
  }

  // WRAPPERS!
  int sutton::update_game(godot::String gameName) {
    return p->update_game(gameName.utf8().get_data());
  }
  int sutton::verify_game(godot::String gameName) {
    return p->verify_game(gameName.utf8().get_data());
  }
  bool sutton::isSDKInstalled(){
    return p->isSDKInstalled();
  };
  bool sutton::isTF2Installed(){
    return p->isTF2Installed();
  };
  int sutton::init_games() {
    return p->init_games();
  };
  godot::String sutton::find_sourcemod_path(){
    auto path = p->find_sourcemod_path();
    return path.c_str();
  };
  godot::String sutton::get_sourcemod_path(){
    return p->sourcemodsPath.c_str();
  };
  // practically a wrapper
  godot::Array sutton::get_server_games() {
    std::vector<std::string> games = p->get_games();
    godot::Array gd_games;
    for(const auto& i: games){
      gd_games.push_back(i.c_str());
    }
    return gd_games;
  }
  void sutton::set_sourcemod_path(godot::String gd_path){
      std::filesystem::path path = std::filesystem::path(gd_path.utf8().get_data());
      if(exists(path)){
      p->sourcemodsPath = path;
      }
  };
  sutton::~sutton(){
    UtilityFunctions::print("bye bye");
  }