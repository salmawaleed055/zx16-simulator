cmake_minimum_required(VERSION 3.15)
project(z16sim CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set SFML directory (note: SFML_DIR, not SFML DIR)
set(SFML_DIR "C:/Users/salma waleed/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/lib/cmake/SFML")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(z16sim
    z16sim.cpp
        graphics.cpp
        backend/z16sim.cpp
        backend/graphics.cpp
)

target_link_libraries(z16sim PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system 
    sfml-audio
)

target_compile_options(z16sim PRIVATE -Wall -Wextra -pedantic)
