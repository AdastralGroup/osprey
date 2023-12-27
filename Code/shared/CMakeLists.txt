project(shared)
add_library(shared adastral_defs.cpp events/error.cpp events/event.cpp events/eventsystem.cpp events/progress.cpp
        events/error.hpp events/progress.hpp events/eventsystem.hpp events/event.hpp)
target_include_directories(shared PUBLIC ./)
target_include_directories(shared PUBLIC ./events)
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