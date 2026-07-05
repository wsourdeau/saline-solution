#pragma once

#include <string>
#include <memory>
#include <vector>

// Include from common library
#include "ChemicalData.h"
#include "Calculations.h"

// Forward declarations for ImGui and GLFW
struct GLFWwindow;

class SalineSolutionApp {
public:
    SalineSolutionApp();
    ~SalineSolutionApp();

    // Main application loop
    void run();

private:
    // GLFW and ImGui setup
    void init_glfw_and_gl();
    void init_imgui(GLFWwindow* window);
    void cleanup();

    // Rendering
    void render_ui();
    void render_main_menu();
    void render_acid_list();
    void render_base_list();
    void render_salt_display();
    void render_results_pane();

    // Application logic
    void calculate();
    void clear_results();
    void clear_selection();
    std::string create_salt_formula(const Base& base, const Acid& acid) const;
    void update_salt_display();
    std::string get_conjugate_base_display(const Acid& acid) const;
    std::string format_summary_results() const;
    std::string format_detailed_results() const;

    // State variables
    GLFWwindow* window_ = nullptr;
    int window_width_ = 1200;
    int window_height_ = 800;

    // Chemical selection
    const Acid* selected_acid_ = nullptr;
    const Base* selected_base_ = nullptr;
    int selected_acid_index_ = -1;
    int selected_base_index_ = -1;

    // Input
    char concentration_input_[64] = "0.1";

    // Results
    std::unique_ptr<CalculationResults> current_results_;
    bool has_results_ = false;
    bool show_details_ = false;

    // UI state
    bool show_acid_list_ = true;
    bool show_base_list_ = true;
    bool show_salt_display_ = true;
    bool show_results_ = true;

    // Scroll positions
    float acid_scroll_ = 0.0f;
    float base_scroll_ = 0.0f;
};
