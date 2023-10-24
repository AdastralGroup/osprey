ad_cmake_init()
project(moss)
ad_create_target(SHARED_LIBRARY ${PROJECT_NAME})
include(FetchContent)
target_include_directories(moss PUBLIC ./)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

##target_link_libraries(${PROJECT_NAME} PUBLIC)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

