ad_cmake_init()
find_package(CURL REQUIRED)
# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)


ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
include(FetchContent)

target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json CURL::libcurl emley )