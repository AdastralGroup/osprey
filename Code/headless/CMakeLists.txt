project(headless)
add_executable(headless headless.cpp)
# Get the name of this folder as the project name
target_link_libraries(${PROJECT_NAME} PRIVATE shared palace)