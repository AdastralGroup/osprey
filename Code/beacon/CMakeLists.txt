ad_cmake_init()
project(beacon)
ad_create_target(APPLICATION beacon)
target_include_directories(beacon PUBLIC ../)
# Get the name of this folder as the project name
target_link_libraries(${PROJECT_NAME} PRIVATE angus palace)