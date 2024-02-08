project(sys)
add_library(${PROJECT_NAME} STATIC sys.cpp zip/zip.c)
target_include_directories(sys PUBLIC ../${PROJECT_NAME})

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
include(FetchContent)
# Get the name of this folder as the project name
IF (GODOT)
    target_include_directories(${PROJECT_NAME} PUBLIC godot::cpp)
    target_link_libraries(${PROJECT_NAME} PUBLIC godot::cpp)
ENDIF ()
get_filename_component(PROJECT_NAME ../${PROJECT_NAME} NAME_WE)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json shared)