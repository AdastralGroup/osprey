project(moss)
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
target_include_directories(moss PUBLIC .)
target_include_directories(moss PUBLIC ../)
# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)