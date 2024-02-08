IF (GODOT)
    FetchContent_Declare(
            GDExtension
            GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
            GIT_TAG godot-4.1-stable
    )
    FetchContent_MakeAvailable(GDExtension)
    add_compile_definitions(GODOT)
    add_subdirectory(binding)
ELSE ()
    add_subdirectory(headless)
ENDIF ()
add_subdirectory(sys)
add_subdirectory(net)
add_subdirectory(emley)
add_subdirectory(shared)
add_subdirectory(palace)