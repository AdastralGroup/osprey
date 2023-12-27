project(binding LANGUAGES C CXX)
include(FetchContent)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

FetchContent_Declare(
        GDExtension
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG godot-4.1-stable
)

IF (WIN32)
ELSE ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBNOTIFY REQUIRED IMPORTED_TARGET libnotify)
    pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
    pkg_check_modules(deps REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
ENDIF ()
set(CMAKE_SHARED_LIBRARY_PREFIX_CXX "")
FetchContent_MakeAvailable(GDExtension)
add_library(${PROJECT_NAME} SHARED binding.cpp register_types.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ../palace)
target_include_directories(${PROJECT_NAME} PUBLIC ../shared)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/"
        OUTPUT_NAME "winter-x64-debug" # make this suck less later
)
IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC shared nlohmann_json::nlohmann_json godot::cpp palace)
ELSE ()
    target_link_libraries(${PROJECT_NAME} PRIVATE shared nlohmann_json::nlohmann_json PkgConfig::LIBNOTIFY PkgConfig::deps godot::cpp palace)
ENDIF ()