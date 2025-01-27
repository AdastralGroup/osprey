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

if (CMAKE_SIZE_OF_VOID_P EQUAL 4)
    set(WINTER_ARCHITECTURE x86)
else()
    set(WINTER_ARCHITECTURE x64)
endif()

FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOLOWER ${config} CONFIG_LOWER)
    string(TOUPPER ${config} CONFIG_UPPER)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_SOURCE_DIR}/../belmont/bin 
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_SOURCE_DIR}/../belmont/bin 
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_SOURCE_DIR}/../belmont/bin 
        ${CONFIG_UPPER}_OUTPUT_NAME winter-${WINTER_ARCHITECTURE}-${CONFIG_LOWER})
ENDFOREACH(config)
    
IF (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC shared nlohmann_json::nlohmann_json godot::cpp palace)
ELSE ()
    target_link_libraries(${PROJECT_NAME} PRIVATE shared nlohmann_json::nlohmann_json PkgConfig::LIBNOTIFY PkgConfig::deps godot::cpp palace)
ENDIF ()