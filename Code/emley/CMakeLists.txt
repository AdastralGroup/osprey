ad_cmake_init()
project(emley)
set(JSON_BuildTests OFF)
ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
include(FetchContent)
target_include_directories(emley PUBLIC ./)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)


target_link_libraries(${PROJECT_NAME} PUBLIC moss)
target_link_libraries(${PROJECT_NAME} PRIVATE sheffield)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif ()

