ad_cmake_init()
project(emley)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(CURL REQUIRED)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
target_include_directories(emley PUBLIC ./)
include(FetchContent)

# Get the name of this folder as the project name
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl nlohmann_json::nlohmann_json fremont bilsdale angus)
