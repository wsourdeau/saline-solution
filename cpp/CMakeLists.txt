cmake_minimum_required(VERSION 3.10)
project(SalineSolution LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add common library subdirectory
add_subdirectory(common)

# Add imgui application subdirectory (optional - only build if GUI is needed)
add_subdirectory(imgui)
