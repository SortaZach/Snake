cmake_minimum_required(VERSION 3.16)
project(Snake)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_executable(Snake WIN32 SDL3_snake.c)

# Link to the actual SDL3 library.
target_link_libraries(Snake PRIVATE SDL3::SDL3)
