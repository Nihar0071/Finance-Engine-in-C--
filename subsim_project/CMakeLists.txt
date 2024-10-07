cmake_minimum_required(VERSION 3.10)
project(MonteCarloSimulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add library sources

add_library(montecarlo STATIC
    src/montecarlo.cpp
)

# Link libraries
target_link_libraries(montecarlo subsim)

# Add executable
add_executable(monte_carlo_example src/example.cpp)
target_link_libraries(monte_carlo_example montecarlo)