cmake_minimum_required(VERSION 3.15)
project(MonteCarloStockSim)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)

# Include vcpkg toolchain file (adjust path as needed)
set(CMAKE_TOOLCHAIN_FILE "/Users/niharpatel/Desktop/Quant/C++ Quant Finance /Finance-Engine-in-C--/packages/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Manually specify the path to xlntConfig.cmake
set(CMAKE_PREFIX_PATH "/Users/niharpatel/Desktop/Quant/C++ Quant Finance /Finance-Engine-in-C--/packages/vcpkg/packages/xlnt_arm64-osx/share/xlnt")

# Manually specify the path to xlnt include directory
include_directories("/Users/niharpatel/Desktop/Quant/C++ Quant Finance /Finance-Engine-in-C--/packages/vcpkg/packages/xlnt_arm64-osx/include")

# Manually specify the path to xlnt library directory
link_directories("/Users/niharpatel/Desktop/Quant/C++ Quant Finance /Finance-Engine-in-C--/packages/vcpkg/packages/xlnt_arm64-osx/lib")

# Find xlnt package and add as a dependency
find_package(xlnt CONFIG REQUIRED)

# Add the executable
add_executable(MonteCarloStockSim main.cpp)

# Link the xlnt library
target_link_libraries(MonteCarloStockSim PRIVATE xlnt)
