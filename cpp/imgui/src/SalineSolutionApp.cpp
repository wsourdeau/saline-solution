#include <iostream>
#include <sstream>
#include <iomanip>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Calculations.h"
#include "ChemicalData.h"

#include "SalineSolutionApp.h"


// GL callback for errors
static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

SalineSolutionApp::SalineSolutionApp() {
    // Set up error callback
    glfwSetErrorCallback(glfw_error_callback);
}

SalineSolutionApp::~SalineSolutionApp() {
    cleanup();
}

void SalineSolutionApp::init_glfw_and_gl() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // GLFW configuration
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    window_ = glfwCreateWindow(window_width_, window_height_, 
                              "⚗️ Saline Solution (C++/ImGui)", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Make context current
    glfwMakeContextCurrent(window_);

    // Initialize GLAD (if available)
    // Note: For this simple example, we'll rely on GLFW's OpenGL context
    // If you need GLAD, uncomment and include glad.h
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     std::cerr << "Failed to initialize GLAD" << std::endl;
    // }

    // Set window icon (if possible)
    // Note: This would require loading an image, which we'll skip for simplicity
}

void SalineSolutionApp::init_imgui(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Set up custom styling to match the Textual TUI look
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 4.0f;

    // Colors - try to match the Textual dark theme
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.26f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.31f, 0.32f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.36f, 0.37f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.26f, 0.27f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.19f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.27f, 0.28f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.32f, 0.33f, 0.34f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.37f, 0.38f, 0.39f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.80f, 0.40f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.80f, 0.40f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.80f, 0.40f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.19f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.36f, 0.37f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.26f, 0.27f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.19f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void SalineSolutionApp::cleanup() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup GLFW
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void SalineSolutionApp::run() {
    try {
        init_glfw_and_gl();
        init_imgui(window_);

        // Main loop
        while (!glfwWindowShouldClose(window_)) {
            // Poll and handle events
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render our UI
            render_ui();

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window_, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.13f, 0.14f, 0.15f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap buffers
            glfwSwapBuffers(window_);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        cleanup();
        throw;
    }
}

void SalineSolutionApp::render_ui() {
    // Set up the main window to cover the entire viewport
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window_width_), 
                                   static_cast<float>(window_height_)));
    
    ImGui::Begin("MainWindow", nullptr, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoCollapse);

    // Render menu bar
    render_main_menu();

    // Main content area
    ImVec2 content_pos = ImGui::GetCursorPos();
    float content_height = ImGui::GetContentRegionAvail().y - content_pos.y;
    
    // Top row: acids, salt display, bases
    float top_row_height = content_height * 0.6f;
    float bottom_row_height = content_height * 0.4f;
    
    // Create a horizontal layout for the top row
    ImGui::BeginChild("TopRow", ImVec2(-1, top_row_height), false);
    
    // Split into three columns
    float column_width = (ImGui::GetContentRegionAvail().x - 2 * ImGui::GetStyle().ItemSpacing.x) / 3.0f;
    
    // Acid list (left)
    ImGui::BeginChild("AcidColumn", ImVec2(column_width, -1), true);
    render_acid_list();
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Salt display (middle)
    ImGui::BeginChild("SaltColumn", ImVec2(column_width, -1), true);
    render_salt_display();
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Base list (right)
    ImGui::BeginChild("BaseColumn", ImVec2(column_width, -1), true);
    render_base_list();
    ImGui::EndChild();
    
    ImGui::EndChild(); // End of TopRow
    
    // Bottom row: Results
    ImGui::BeginChild("ResultsRow", ImVec2(-1, bottom_row_height), true);
    render_results_pane();
    ImGui::EndChild();

    ImGui::End(); // End of MainWindow
}

void SalineSolutionApp::render_main_menu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
                glfwSetWindowShouldClose(window_, true);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Acid List", nullptr, &show_acid_list_);
            ImGui::MenuItem("Base List", nullptr, &show_base_list_);
            ImGui::MenuItem("Salt Display", nullptr, &show_salt_display_);
            ImGui::MenuItem("Results Pane", nullptr, &show_results_);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            ImGui::TextDisabled("Saline Solution Calculator");
            ImGui::TextDisabled("C++/ImGui version");
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void SalineSolutionApp::render_acid_list() {
    if (!show_acid_list_) return;
    
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
    ImGui::BeginChild("AcidList", ImVec2(-1, -1), true, 
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Acids");
    ImGui::Separator();
    
    // List all acids
    ImGuiListClipper clipper;
    clipper.Begin(ACIDS.size());
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            const Acid& acid = ACIDS[i];
            bool is_selected = (selected_acid_ == &acid);
            
            // Highlight selected acid
            if (is_selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.4f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
            }
            
            if (ImGui::Button(acid.name.c_str(), ImVec2(-1, 0))) {
                selected_acid_ = &acid;
                selected_acid_index_ = i;
                update_salt_display();
                clear_results();
            }
            
            ImGui::PopStyleColor(3);
            
            // Show pKa and valence as hint
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("pKa: %.2f", acid.pKa);
                ImGui::Text("Valence: %d", acid.vAc);
                ImGui::EndTooltip();
            }
        }
    }
    clipper.End();
    
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void SalineSolutionApp::render_base_list() {
    if (!show_base_list_) return;
    
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.4f, 0.8f, 0.8f));
    ImGui::BeginChild("BaseList", ImVec2(-1, -1), true, 
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    ImGui::TextColored(ImVec4(0.4f, 0.6f, 1.0f, 1.0f), "Bases");
    ImGui::Separator();
    
    // List all bases
    ImGuiListClipper clipper;
    clipper.Begin(BASES.size());
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            const Base& base = BASES[i];
            bool is_selected = (selected_base_ == &base);
            
            // Highlight selected base
            if (is_selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.4f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
            }
            
            if (ImGui::Button(base.name.c_str(), ImVec2(-1, 0))) {
                selected_base_ = &base;
                selected_base_index_ = i;
                update_salt_display();
                clear_results();
            }
            
            ImGui::PopStyleColor(3);
            
            // Show pKb and valence as hint
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("pKb: %.2f", base.pKb);
                ImGui::Text("Valence: %d", base.vBa);
                ImGui::EndTooltip();
            }
        }
    }
    clipper.End();
    
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void SalineSolutionApp::render_salt_display() {
    if (!show_salt_display_) return;
    
    ImGui::BeginChild("SaltDisplay", ImVec2(-1, -1), true);
    
    ImGui::Text("Salt Formation");
    ImGui::Separator();
    
    // Salt formula display
    if (selected_acid_ && selected_base_) {
        std::string salt_formula = create_salt_formula(*selected_base_, *selected_acid_);
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "Salt: %s", salt_formula.c_str());
    } else if (selected_acid_) {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.4f, 1.0f), "Salt: %s + ...", selected_acid_->name.c_str());
    } else if (selected_base_) {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.4f, 1.0f), "Salt: ... + %s", selected_base_->name.c_str());
    } else {
        ImGui::TextDisabled("Select one acid and one base");
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Concentration input
    ImGui::Text("Concentration:");
    bool input_enabled = (selected_acid_ && selected_base_);
    if (!input_enabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }
    
    if (ImGui::InputText("##concentration", concentration_input_, sizeof(concentration_input_))) {
        clear_results();
    }
    
    if (!input_enabled) {
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();
    }
    
    ImGui::Text("M");
    ImGui::SameLine();
    ImGui::TextDisabled("(Molar)");
    
    ImGui::Spacing();
    
    // Calculate button
    bool can_calculate = (selected_acid_ && selected_base_ && 
                         str_is_float(concentration_input_));
    
    if (can_calculate) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.8f, 0.4f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.9f, 0.5f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
    }
    
    if (ImGui::Button("Calculate pH", ImVec2(-1, 0)) && can_calculate) {
        calculate();
    }
    
    ImGui::PopStyleColor(3);
    
    // Keyboard shortcut for calculate
    if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) {
        if (can_calculate) {
            calculate();
        }
    }
    
    ImGui::EndChild();
}

void SalineSolutionApp::render_results_pane() {
    if (!show_results_) return;
    
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.8f, 0.8f, 0.8f));
    ImGui::BeginChild("ResultsPane", ImVec2(-1, -1), true);
    
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.8f, 1.0f), "Calculation Results");
    ImGui::Separator();
    
    if (!has_results_) {
        ImGui::TextDisabled("No results to display. Select an acid, a base, enter concentration, and click Calculate.");
    } else {
        // Toggle for more details
        bool details_toggled = false;
        if (ImGui::Checkbox("Show more details", &show_details_)) {
            details_toggled = true;
        }
        
        ImGui::Separator();
        
        if (show_details_) {
            ImGui::TextWrapped("%s", format_detailed_results().c_str());
        } else {
            ImGui::TextWrapped("%s", format_summary_results().c_str());
        }
        
        // Auto-scroll to top when results change or details toggle
        if (details_toggled) {
            ImGui::SetScrollHereY(0.0f);
        }
    }
    
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void SalineSolutionApp::calculate() {
    if (!selected_acid_ || !selected_base_) {
        return;
    }
    
    // Create salt from selected acid and base
    std::string formula = create_salt_formula(*selected_base_, *selected_acid_);
    Salt salt(formula, *selected_acid_, *selected_base_);
    
    // Get concentration
    double concentration = std::atof(concentration_input_);
    if (concentration <= 0) {
        // Handle invalid concentration
        return;
    }
    
    // Calculate
    current_results_ = calculate_results(salt, concentration);
    has_results_ = true;
    show_details_ = false; // Start with summary view
}

void SalineSolutionApp::clear_results() {
    has_results_ = false;
    show_details_ = false;
}

void SalineSolutionApp::clear_selection() {
    selected_acid_ = nullptr;
    selected_base_ = nullptr;
    selected_acid_index_ = -1;
    selected_base_index_ = -1;
    clear_results();
}

std::string SalineSolutionApp::create_salt_formula(const Base& base, const Acid& acid) const {
    int grac = count_uppers(acid.name) > 1;
    auto open_paren_acid = VISIBLE_PARENS.first;
    auto close_paren_acid = VISIBLE_PARENS.second;
    if (base.vBa != 1 && grac) {
        open_paren_acid = VISIBLE_PARENS.first;
        close_paren_acid = VISIBLE_PARENS.second;
    } else {
        open_paren_acid = HIDDEN_PARENS.first;
        close_paren_acid = HIDDEN_PARENS.second;
    }
    
    int grba = count_uppers(base.name) > 1;
    auto open_paren_base = VISIBLE_PARENS.first;
    auto close_paren_base = VISIBLE_PARENS.second;
    if (acid.vAc != 1 && grba) {
        open_paren_base = VISIBLE_PARENS.first;
        close_paren_base = VISIBLE_PARENS.second;
    } else {
        open_paren_base = HIDDEN_PARENS.first;
        close_paren_base = HIDDEN_PARENS.second;
    }
    
    std::string base_valence_str = create_valence_str(base.vBa);
    std::string acid_valence_str = create_valence_str(acid.vAc);
    
    return open_paren_acid + acid.name + close_paren_acid + base_valence_str +
           open_paren_base + base.name + close_paren_base + acid_valence_str;
}

void SalineSolutionApp::update_salt_display() {
    // This function is called when acid or base selection changes
    // In ImGui, we don't need to do much here as the rendering is done each frame
}

std::string SalineSolutionApp::format_summary_results() const {
    if (!has_results_) return "";
    
    const auto & r = current_results_;
    int dr_index = static_cast<int>(r->dominant_reaction);
    
    std::ostringstream oss;
    oss << "Dominant Reaction: " << MSGS[dr_index] << "\n\n";
    
    oss << std::fixed << std::setprecision(4);
    oss << "┌─────────────────┬───────────┐\n";
    oss << "│ Property        │ Value     │\n";
    oss << "├─────────────────┼───────────┤\n";
    oss << "│ pH              │ " << std::setw(10) << r->pH << " │\n";
    oss << "│ pOH             │ " << std::setw(10) << r->pOH << " │\n";
    
    oss << std::scientific << std::setprecision(2);
    oss << "│ [H₃O⁺]          │ " << std::setw(10) << r->H3O << " M │\n";
    oss << "│ [OH⁻]           │ " << std::setw(10) << r->OH << " M │\n";
    oss << "│ inv_Kh          │ " << std::setw(10) << r->inv_Kh << " │\n";
    
    oss << std::fixed << std::setprecision(4);
    oss << "│ Hydrolysis (x)  │ " << std::setw(10) << r->x << " │\n";
    oss << "└─────────────────┴───────────┘\n";
    
    return oss.str();
}

std::string SalineSolutionApp::format_detailed_results() const {
    if (!has_results_) return "";
    
    const auto& r = current_results_;
    int dr_index = static_cast<int>(r->dominant_reaction);
    
    std::ostringstream oss;
    oss << "Dominant Reaction: " << MSGS[dr_index] << "\n\n";
    
    oss << std::fixed << std::setprecision(4);
    oss << "┌─────────────────┬───────────┐\n";
    oss << "│ Property        │ Value     │\n";
    oss << "├─────────────────┼───────────┤\n";
    oss << "│ pH              │ " << std::setw(10) << r->pH << " │\n";
    oss << "│ pOH             │ " << std::setw(10) << r->pOH << " │\n";
    
    oss << std::scientific << std::setprecision(2);
    oss << "│ [H₃O⁺]          │ " << std::setw(10) << r->H3O << " M │\n";
    oss << "│ [OH⁻]           │ " << std::setw(10) << r->OH << " M │\n";
    oss << "│ inv_Kh          │ " << std::setw(10) << r->inv_Kh << " │\n";
    
    oss << std::fixed << std::setprecision(4);
    oss << "│ Hydrolysis (x)  │ " << std::setw(10) << r->x << " │\n";
    oss << "└─────────────────┴───────────┘\n\n";
    
    // All reactions
    oss << "All Reactions:\n";
    oss << "────────────────────────────────────────────────────\n";
    
    for (const auto& rxn : r->reactions) {
        bool is_dominant = (rxn.index == dr_index);
        std::string marker = is_dominant ? "[✓] " : "[ ] ";
        
        oss << marker << "Reaction " << (rxn.index + 1) << ": " << rxn.description << "\n";
        oss << "   Equation: " << rxn.equation << "\n";
        oss << std::scientific << std::setprecision(2);
        oss << "   " << rxn.constant_name << ": " << rxn.constant_value << "\n";
        oss << "\n";
    }
    
    oss << "Dominant Reaction Details:\n";
    oss << "────────────────────────────────────────────────────\n";
    
    if (r->remaining_ion_concentration) {
        std::string acid_charge = create_valence_str(r->salt.acid.vAc);
        oss << "- [" << r->salt.acid.name << acid_charge << "] = " 
            << std::setprecision(4) << *r->remaining_ion_concentration << " M\n";
    }
    
    if (r->product_concentration) {
        std::string conjugate_base = get_conjugate_base_display(r->salt.acid);
        oss << "- [" << conjugate_base << "] = " 
            << std::setprecision(4) << *r->product_concentration << " M\n";
    }
    
    if (r->dr_H3O) {
        oss << std::scientific << std::setprecision(2);
        oss << "- [H₃O⁺] = " << *r->dr_H3O << " M\n";
    }
    
    if (r->dr_OH) {
        oss << std::scientific << std::setprecision(2);
        oss << "- [OH⁻] = " << *r->dr_OH << " M\n";
    }
    
    return oss.str();
}

std::string SalineSolutionApp::get_conjugate_base_display(const Acid& acid) const {
    if (has_hydrogen(acid.name)) {
        if (acid.name == "NH₄") {
            return "NH₃";
        }
        if (!acid.name.empty()) {
            return acid.name.substr(0, acid.name.size() - 1);
        }
    }
    
    // For metal cations
    int charge = acid.vAc - 1;
    std::string charge_str = get_ionic_charge_str('+', charge);
    return acid.name + "OH" + charge_str;
}
