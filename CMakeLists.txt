cmake_minimum_required (VERSION 3.8)

project(adastral)
add_subdirectory(adastral)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY , ${CMAKE_BINARY_DIR} + "/bin")

add_definitions(-DCURL_STATICLIB)

