ad_cmake_init()
project(headless)
ad_create_target(APPLICATION headless)
# Get the name of this folder as the project name
target_link_libraries(${PROJECT_NAME} PRIVATE shared palace)