cmake_minimum_required(VERSION 3.30)

project(Shad)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE readline)

configure_file(
    "scripts/build_config.sh"   
    "${CMAKE_BINARY_DIR}/build_config.sh"  
    @ONLY
)

