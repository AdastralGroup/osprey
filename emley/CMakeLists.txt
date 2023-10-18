cmake_minimum_required (VERSION 3.8)

project(adastral)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(adastral)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR} + "/bin")

