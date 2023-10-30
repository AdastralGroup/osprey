ad_cmake_init()
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
ad_create_target(STATIC_LIBRARY ${PROJECT_NAME})
target_include_directories(sutton PUBLIC ../)
# Get the name of this folder as the project name
FetchContent_MakeAvailable(GDExtension)


target_link_libraries(${PROJECT_NAME} PUBLIC coldfield)
