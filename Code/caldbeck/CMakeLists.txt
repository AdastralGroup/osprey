project(caldbeck)
ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
target_include_directories(caldbeck PUBLIC .)
# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)