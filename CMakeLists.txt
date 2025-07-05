cmake_minimum_required(VERSION 3.15)
project(zx16_simulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Your executables
add_executable(zx16_simulator z16sim.cpp)
add_executable(Create_Test_bins z16sim.cpp create_test_bins.cpp
)
add_executable(zx16_simulator_tests Tests.cpp z16sim.cpp)
add_executable(zx16_simulator_tests_2 z16sim.cpp)

# Link SFML to ALL executables that use z16sim.cpp
#target_link_libraries(zx16_simulator PRIVATE sfml-graphics sfml-window sfml-system)
# target_link_libraries(Create_Test_bins PRIVATE sfml-graphics sfml-window sfml-system)
# target_link_libraries(zx16_simulator_tests PRIVATE sfml-graphics sfml-window sfml-system)
# target_link_libraries(zx16_simulator_tests_2 PRIVATE sfml-graphics sfml-window sfml-system)

# Compile options
target_compile_options(zx16_simulator PRIVATE -Wall -Wextra -pedantic)
# target_compile_options(zx16_simulator_tests PRIVATE -Wall -Wextra -pedantic)
# target_compile_options(Create_Test_bins PRIVATE -Wall -Wextra -pedantic)
# target_compile_options(zx16_simulator_tests_2 PRIVATE -Wall -Wextra -pedantic)
