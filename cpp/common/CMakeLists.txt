cmake_minimum_required(VERSION 3.10)

# Create static library
add_library(saline_common STATIC
    src/ChemicalData.cpp
    src/Calculations.cpp
)

# Include directories - make headers available to users of this library
target_include_directories(saline_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler flags for this library
target_compile_options(saline_common PRIVATE
    -finput-charset=utf-8
    -O3
    -Wall
    -pedantic
)
