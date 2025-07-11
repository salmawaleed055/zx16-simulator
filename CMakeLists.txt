cmake_minimum_required(VERSION 3.15)
project(z16sim CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set SFML directory (note: SFML_DIR, not SFML DIR)
set(SFML_DIR "C:/Users/salma waleed/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/lib/cmake/SFML")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)
# Add executable
add_executable(z16sim
        z16sim.cpp
        graphics.cpp
)

# Link SFML libraries
target_link_libraries(z16sim PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(z16sim PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)
# Add compile options for warnings (optional)
target_compile_options(z16sim PRIVATE -Wall -Wextra -pedantic)

# Post build step to copy the necessary DLLs
add_custom_command(TARGET z16sim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "C:/Users/salma waleed/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/bin/sfml-graphics-2.dll"
        "C:/Users/salma waleed/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/bin/sfml-window-2.dll"
        "C:/Users/salma waleed/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/bin/sfml-system-2.dll"
        $<TARGET_FILE_DIR:z16sim>  # This copies DLLs to the same folder as the executable
)

# Optional: Add all DLLs from the SFML bin directory (if you have more DLLs that you want to copy)
add_custom_command(TARGET z16sim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/Users/salma waleed/Downloads/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1/bin"
        $<TARGET_FILE_DIR:z16sim>
)
