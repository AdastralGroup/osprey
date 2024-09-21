project(torrent)
find_package(CURL REQUIRED)
find_package(LibtorrentRasterbar CONFIG REQUIRED)
#find_package(boost REQUIRED)
add_library(torrent torrent.cpp)
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)


target_include_directories(torrent PUBLIC .)
target_include_directories(torrent PUBLIC ../net)

# Get the name of this folder as the project name
# get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)


target_link_libraries(torrent PRIVATE CURL::libcurl nlohmann_json::nlohmann_json LibtorrentRasterbar::torrent-rasterbar net)