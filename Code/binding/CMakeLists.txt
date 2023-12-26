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

FetchContent_MakeAvailable(GDExtension)

ad_create_target(SHARED_LIBRARY ${PROJECT_NAME})

IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC shared nlohmann_json::nlohmann_json godot::cpp palace)
ELSE ()
    target_link_libraries(${PROJECT_NAME} PRIVATE shared nlohmann_json::nlohmann_json PkgConfig::LIBNOTIFY PkgConfig::deps godot::cpp palace)
ENDIF ()