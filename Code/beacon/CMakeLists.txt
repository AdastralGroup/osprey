ad_cmake_init()
project(beacon)
add_executable(beacon)
target_sources(beacon PUBLIC beacon.cpp)
include(FetchContent)
# Get the name of this folder as the project name
target_link_libraries(${PROJECT_NAME} PRIVATE palace)