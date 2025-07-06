cmake_minimum_required(VERSION 3.15)
project(z16sim CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(z16sim
    z16sim.cpp
)

target_link_libraries(z16sim PRIVATE sfml-graphics sfml-window sfml-system)

target_compile_options(z16sim PRIVATE -Wall -Wextra -pedantic)
