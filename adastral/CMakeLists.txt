add_executable (adastral "adastral.cpp" "version/version.cpp" "kachemak/kachemak.cpp" "utility/utility.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET adastral PROPERTY CXX_STANDARD 20)
endif()

add_compile_definitions(LIBARCHIVE_STATIC)


add_subdirectory(json)
add_subdirectory(libarchive)
add_subdirectory(zstd/build/cmake)

include_directories(./)


target_link_libraries(adastral PRIVATE archive_static libzstd_static)
