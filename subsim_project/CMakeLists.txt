cmake_minimum_required(VERSION 3.15)
project(subsim_openspiel_integration)

# Set the CMAKE_PREFIX_PATH to the OpenSpiel build directory
set(CMAKE_PREFIX_PATH "/Users/niharpatel/Desktop/Boston University Courses/open_spiel/build")

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSpiel package
# find_package(OpenSpiel REQUIRED)

# Your original library (subsim)
add_library(subsim_lib
    src/subsim.cpp
    src/montecarlo.cpp
)

# Include directories for the subsim library
target_include_directories(subsim_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# # Integration test executable for OpenSpiel
# add_executable(openspiel_test
#     src/openspiel_test.cpp
# )

# # Link the subsim library and OpenSpiel to the test executable
# target_link_libraries(openspiel_test
#     PRIVATE
#     subsim_lib
#     open_spiel::open_spiel
# )
