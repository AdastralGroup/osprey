ad_cmake_init()
project(angus)
ad_create_target(STATIC_LIBRARY angus)
target_include_directories(angus PUBLIC ../)
IF(GODOT)
    FetchContent_Declare(
            GDExtension
            GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
            GIT_TAG godot-4.1-stable
    )
    target_compile_definitions(angus PUBLIC GODOT)
    FetchContent_MakeAvailable(GDExtension)
    target_link_libraries(angus PUBLIC godot::cpp)
ENDIF()