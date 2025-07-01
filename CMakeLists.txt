#cmake_minimum_required(VERSION 3.15)
#project(zx16_simulator CXX)
#
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
#add_executable(zx16_simulator
#        z16sim.cpp
#)
#add_executable(Create_Test_bins
#
#        z16sim.cpp
#        create_test_bins.cpp
#)
#add_executable(zx16_simulator_tests
#        Tests.cpp
#        z16sim.cpp
#)
#add_executable(zx16_simulator_tests_2
#        Test_driver.cpp
#        z16sim.cpp
#)
#
#target_compile_options(zx16_simulator PRIVATE -Wall -Wextra -pedantic)
#target_compile_options(zx16_simulator_tests PRIVATE -Wall -Wextra -pedantic)

cmake_minimum_required(VERSION 3.15)
project(zx16_simulator)

set(CMAKE_CXX_STANDARD 20)

# Linux/macOS: Use system-wide SFML
if(UNIX AND NOT APPLE)
    find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
elseif(APPLE)
    find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
else()
    # Windows: Auto-download SFML
    include(FetchContent)
    FetchContent_Declare(
            SFML
            URL https://github.com/SFML/SFML/releases/download/2.5.1/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip
    )
    FetchContent_MakeAvailable(SFML)
endif()

# Build simulator
add_executable(zx16_simulator src/main.cpp src/z16sim.cpp)
target_link_libraries(zx16_simulator
        sfml-graphics
        sfml-audio
        sfml-system
)

# Copy DLLs (Windows only)
if(WIN32)
    add_custom_command(TARGET zx16_simulator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${SFML_BINARY_DIR}/lib/sfml-audio-2.dll" "$<TARGET_FILE_DIR:zx16_simulator>"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${SFML_BINARY_DIR}/lib/sfml-graphics-2.dll" "$<TARGET_FILE_DIR:zx16_simulator>"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${SFML_BINARY_DIR}/lib/sfml-system-2.dll" "$<TARGET_FILE_DIR:zx16_simulator>"
    )
endif()