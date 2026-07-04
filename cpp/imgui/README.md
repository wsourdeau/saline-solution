# Saline Solution Calculator - C++/ImGui Version

A visual and functional equivalent of the Python Textual TUI version of Saline Solution, implemented in C++ using the Dear ImGui library with GLFW and OpenGL3 backend.

## Features

- **Visual Layout**: Three-column layout matching the Textual TUI
  - Left: Acid selection list (red accent)
  - Middle: Salt formation display and concentration input
  - Right: Base selection list (blue accent)
  - Bottom: Results pane (cyan accent)

- **Functionality**:
  - Select acids from the predefined list
  - Select bases from the predefined list
  - Automatic salt formula generation
  - Concentration input in Molar (M)
  - pH calculation with multiple reaction types
  - Toggle between summary and detailed results
  - Keyboard shortcuts (Enter to calculate, Ctrl+Q to quit)

- **Chemical Calculations**:
  - Acid hydrolysis constants (Ka)
  - Base hydrolysis constants (Kb)
  - Salt hydrolysis constants (Kh)
  - Water autoionization (Kw)
  - Dominant reaction determination
  - pH, pOH, ion concentrations, hydrolysis degree

## Project Structure

```
cpp/
├── CMakeLists.txt          # CMake build configuration
├── README.md              # This file
├── include/
│   ├── ChemicalData.h     # Chemical data types and constants
│   ├── Calculations.h     # Chemical calculation declarations
│   └── SalineSolutionApp.h # Main application class
└── src/
    ├── main.cpp            # Application entry point
    ├── ChemicalData.cpp    # Chemical data implementations
    ├── Calculations.cpp    # Chemical calculation implementations
    └── SalineSolutionApp.cpp # Main application implementation
```

## Dependencies

- **GLFW 3.3+** - Window creation and OpenGL context management
- **OpenGL 3.3+** - Graphics rendering
- **Dear ImGui v1.89.9** - Immediate mode GUI library
- **CMake 3.10+** - Build system
- **C++17** - Modern C++ features

### Platform-specific Dependencies

- **Linux**: `libglfw3-dev`, `libgl1-mesa-dev`, `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev`
- **macOS**: GLFW and OpenGL are typically available through Homebrew (`brew install glfw`)
- **Windows**: GLFW and OpenGL development libraries

## Building

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake libglfw3-dev libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# Build the project
mkdir build
cd build
cmake ..
make -j$(nproc)

# Run the application
./bin/saline_solution_cpp
```

### macOS

```bash
# Install dependencies (if using Homebrew)
brew install cmake glfw

# Build the project
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)

# Run the application
./bin/saline_solution_cpp
```

### Windows (MSYS2/MinGW)

```bash
# Install dependencies (using MSYS2)
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-glfw mingw-w64-x86_64-mesa

# Build the project
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release

# Run the application
./bin/saline_solution_cpp.exe
```

## Usage

1. **Select an Acid**: Click on any acid from the left list (highlighted in red when selected)
2. **Select a Base**: Click on any base from the right list (highlighted in blue when selected)
3. **Set Concentration**: Enter a positive concentration value in Molar (M)
4. **Calculate**: Click "Calculate pH" or press Enter
5. **View Results**: 
   - Summary view shows basic properties
   - Check "Show more details" for comprehensive reaction information

## Keyboard Shortcuts

- **Enter**: Calculate pH (when valid selection and concentration)
- **Ctrl+Q**: Exit the application
- **Escape**: Clear current selection

## Chemical Data

The application includes the same chemical dataset as the Python version:

### Acids (11)
K, Na, NH₄, Ca, Pb, Cd, Cu, Zn, Sn, Al, Cr

### Bases (11)
Cl, Br, F, I, NO₂, CN, CH₃COO, S, PO₄, CO₃, AsO₄

## Implementation Details

### Architecture
- **Class-based Design**: `SalineSolutionApp` manages the entire application
- **Separation of Concerns**: Chemical data, calculations, and UI are separated
- **Modern C++**: Uses C++17 features like `std::optional`, structured bindings, etc.

### UI Components
- **MainWindow**: Full-screen window with menu bar
- **AcidList**: Scrollable list with selection highlighting and tooltips
- **BaseList**: Scrollable list with selection highlighting and tooltips
- **SaltDisplay**: Shows the formed salt formula and concentration input
- **ResultsPane**: Displays calculation results with toggleable detail level

### Chemical Calculations
The calculations follow the same algorithms as the Python version:
1. Build all possible reactions (acid hydrolysis, base hydrolysis, salt hydrolysis, water autoionization)
2. Determine dominant reaction by comparing constants
3. Calculate pH, pOH, ion concentrations based on dominant reaction
4. Compute hydrolysis degree and remaining ion concentrations

## Troubleshooting

### Common Issues

1. **GLFW not found**: Ensure GLFW development libraries are installed
2. **OpenGL not found**: Install appropriate OpenGL development packages
3. **ImGui download fails**: Check internet connection and GitHub accessibility
4. **Missing GLAD**: This implementation doesn't require GLAD; it uses GLFW's built-in OpenGL support

### Debugging

Add `-DCMAKE_BUILD_TYPE=Debug` to cmake command for debug builds:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## License

This code is provided as a functional equivalent to the Python Textual TUI version and follows the same licensing terms.

## Contributing

Feel free to submit pull requests for improvements, bug fixes, or additional features.