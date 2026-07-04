#include <GLFW/glfw3.h>
#include <iostream>

// Include our application
#include "SalineSolutionApp.h"

int main(int argc, char* argv[]) {
    std::cout << "Saline Solution Calculator (C++/ImGui)" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // Create and run the application
        SalineSolutionApp app;
        app.run();
        
        std::cout << "Application exited normally." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}