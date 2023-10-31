ad_cmake_init()
find_package(CURL REQUIRED)
find_package(LibtorrentRasterbar REQUIRED)
find_package(boost REQUIRED)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
include(FetchContent)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ../)
# Get the name of this folder as the project name



target_link_libraries(${PROJECT_NAME} PUBLIC fremont CURL::libcurl nlohmann_json::nlohmann_json)
