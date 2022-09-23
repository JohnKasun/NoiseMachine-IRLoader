cmake_minimum_required(VERSION 3.15)
Project(NoiseMachine-IRLoader VERSION 1.0.4)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(JUCE)
add_subdirectory(src bin)
add_subdirectory(cmake)
add_subdirectory(test)
add_subdirectory(extern)

