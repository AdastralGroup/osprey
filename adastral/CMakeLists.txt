add_executable (adastral "adastral.cpp" "version/version.cpp" "kachemak/kachemak.cpp" "utility/utility.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET adastral PROPERTY CXX_STANDARD 20)
endif()

add_subdirectory(json)

include_directories(./)
