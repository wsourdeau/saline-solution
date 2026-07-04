cmake_minimum_required(VERSION 3.10)
project(SalineSolutionCPP)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GLFW and OpenGL
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(include)

# Add the executable
add_executable(saline_solution_cpp
    src/main.cpp
    src/ChemicalData.cpp
    src/Calculations.cpp
    src/SalineSolutionApp.cpp
)

target_include_directories(saline_solution_cpp PRIVATE  /usr/include/imgui)

# Link imgui to our executable
target_link_libraries(saline_solution_cpp PRIVATE imgui)

# # Add the imgui source files
# target_sources(saline_solution_cpp PRIVATE 
#     ${imgui_SOURCE_DIR}/imgui.cpp
#     ${imgui_SOURCE_DIR}/imgui_draw.cpp
#     ${imgui_SOURCE_DIR}/imgui_tables.cpp
#     ${imgui_SOURCE_DIR}/imgui_widgets.cpp
# )

# Include directories for imgui
# target_include_directories(saline_solution_cpp PRIVATE ${imgui_SOURCE_DIR})

# Platform-specific libraries
if(APPLE)
    target_link_libraries(saline_solution_cpp PRIVATE "-framework Cocoa -framework OpenGL -framework IOKit")
elif(UNIX AND NOT APPLE)
    target_link_libraries(saline_solution_cpp PRIVATE X11)
endif()

# Common libraries
target_link_libraries(saline_solution_cpp PRIVATE glfw OpenGL::GL stb)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
