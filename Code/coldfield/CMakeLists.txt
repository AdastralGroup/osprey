ad_cmake_init()

include(FetchContent)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

FetchContent_Declare(
        GDExtension
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG godot-4.0-stable
)

FetchContent_MakeAvailable(GDExtension)

ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC godot::cpp)
