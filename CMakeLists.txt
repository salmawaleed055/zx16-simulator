#cmake_minimum_required(VERSION 3.30)
#project(zx16_simulator)
#
#set(CMAKE_CXX_STANDARD 20)
#
#add_executable(zx16_simulator
#        main.cpp
#)
cmake_minimum_required(VERSION 3.29.6) # <-- Change this line
project(zx16_simulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(zx16_simulator_run
        z16sim.cpp
        main.cpp
        Tests.cpp

)

add_executable(zx16_simulator_tests
        Tests.cpp
        z16sim.cpp
)


target_compile_options(zx16_simulator_run PRIVATE -Wall -Wextra -pedantic)
target_compile_options(zx16_simulator_tests PRIVATE -Wall -Wextra -pedantic)