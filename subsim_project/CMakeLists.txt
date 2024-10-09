cmake_minimum_required(VERSION 3.10)
project(MonteCarloRL)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/example.cpp
    src/subsim.cpp
    src/montecarlo.cpp
)

# Create executable
add_executable(monte_carlo_rl ${SOURCES})

# Link libraries if any (e.g., pthread)
if(UNIX)
    target_link_libraries(monte_carlo_rl pthread)
endif()
