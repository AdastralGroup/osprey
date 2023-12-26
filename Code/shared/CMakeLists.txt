ad_cmake_init()
project(shared)
ad_create_target(STATIC_LIBRARY shared)
target_include_directories(shared PUBLIC ./)
IF (GODOT)
    FetchContent_Declare(
            GDExtension
            GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
            GIT_TAG godot-4.1-stable
    )
    target_compile_definitions(shared PUBLIC GODOT)
    FetchContent_MakeAvailable(GDExtension)
    target_link_libraries(shared PUBLIC godot::cpp)
ENDIF ()