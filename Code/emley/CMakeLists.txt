ad_cmake_init()
project(emley)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
include(FetchContent)
target_include_directories(emley PUBLIC ./)

# Get the name of this folder as the project name
target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl nlohmann_json::nlohmann_json moss sheffield)
