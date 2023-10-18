add_executable (adastral "adastral.cpp" "version/version.cpp" "kachemak/kachemak.cpp" "utility/utility.cpp" "events/eventsystem.cpp" "events/event.cpp" "events/progress.cpp" "events/error.cpp")

add_compile_definitions(LIBARCHIVE_STATIC)


add_subdirectory(json)
add_subdirectory(libarchive)
add_subdirectory(zstd/build/cmake)

include_directories(./)


target_link_libraries(adastral PRIVATE archive_static libzstd_static)
