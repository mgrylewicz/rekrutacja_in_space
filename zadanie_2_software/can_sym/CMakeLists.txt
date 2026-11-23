cmake_minimum_required(VERSION 3.16)

project(CANSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(can_sim
    src/main.cpp
    src/CANSimulator.cpp
)

target_include_directories(can_sim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(can_sim PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)