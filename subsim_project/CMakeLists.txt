cmake_minimum_required(VERSION 3.10)
project(SubSimulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add executable
add_executable(subsim_example src/main.cpp)