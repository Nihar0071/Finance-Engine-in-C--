cmake_minimum_required(VERSION 3.10)
project(subsim_project)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set OpenSpiel path
set(OPEN_SPIEL_PATH "/Users/niharpatel/Desktop/Quant/C++ Quant Finance /Finance-Engine-in-C--/open_spiel")

# Print paths for debugging
message(STATUS "Project source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Project include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "OpenSpiel dir: ${OPEN_SPIEL_PATH}")

# Add include directories globally
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # This makes "openspiel_adapter.hpp" findable
    ${OPEN_SPIEL_PATH}
    ${OPEN_SPIEL_PATH}/open_spiel/abseil-cpp
)

# Find OpenSpiel shared library
# find_library(OPEN_SPIEL_LIB
#     NAMES 
#         open_spiel
#         libopen_spiel
#     PATHS 
#         "${OPEN_SPIEL_PATH}/build"
#     NO_DEFAULT_PATH
#     REQUIRED
# )

message(STATUS "OpenSpiel library found at: ${OPEN_SPIEL_LIB}")

# Create your library
add_library(subsim_lib
    src/subsim.cpp
    src/montecarlo.cpp
)

# Set include directories for subsim_lib
target_include_directories(subsim_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Create the executable
add_executable(subsim_executable
    src/openspiel_test.cpp
)

# Link libraries
target_link_libraries(subsim_executable
    PRIVATE
    subsim_lib
    ${OPEN_SPIEL_LIB}
)

# Add compiler flags
if(UNIX)
    target_compile_options(subsim_executable PRIVATE -Wall -Wextra)
endif()

# Print include directories for verification
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "Include dir: ${dir}")
endforeach()