cmake_minimum_required(VERSION 3.30)

project(Shad)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) 

add_executable(shell ${SOURCE_FILES})

target_link_libraries(shell PRIVATE readline)
