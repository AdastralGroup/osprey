project(headless)
add_executable(headless headless.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ../palace)
target_include_directories(${PROJECT_NAME} PUBLIC ../shared)
# Get the name of this folder as the project name
target_link_libraries(${PROJECT_NAME} PRIVATE shared palace)