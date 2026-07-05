cmake_minimum_required(VERSION 3.10)

# Find GLFW and OpenGL
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories for this subdirectory
include_directories(include)

# Add the executable (only GUI-specific files)
add_executable(saline_solution_imgui
    src/main.cpp
    src/SalineSolutionApp.cpp
)

# Include directories: imgui headers and common library headers
target_include_directories(saline_solution_imgui PRIVATE 
    /usr/include/imgui
    ${CMAKE_SOURCE_DIR}/common/include
)

# Link libraries: imgui, common library, and dependencies
target_link_libraries(saline_solution_imgui PRIVATE 
    imgui
    saline_common
    glfw OpenGL::GL stb
)

# Compiler flags for this executable
target_compile_options(saline_solution_imgui PRIVATE
    -finput-charset=utf-8
    -O3
    -Werror
    -Wall
    -pedantic
)

# Platform-specific libraries
if(APPLE)
    target_link_libraries(saline_solution_imgui PRIVATE "-framework Cocoa -framework OpenGL -framework IOKit")
elif(UNIX AND NOT APPLE)
    target_link_libraries(saline_solution_imgui PRIVATE X11)
endif()

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
