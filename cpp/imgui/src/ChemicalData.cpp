#include "ChemicalData.h"
#include <cctype>
#include <algorithm>

// Global chemical data initialization
const std::vector<Acid> ACIDS = {
    {"K", 15.6, 1},
    {"Na", 14.8, 1},
    {"NH₄", 9.2, 1},
    {"Ca", 12.7, 2},
    {"Pb", 8.8, 2},
    {"Cd", 8.5, 2},
    {"Cu", 8.0, 2},
    {"Zn", 7.9, 2},
    {"Sn", 1.7, 2},
    {"Al", 4.0, 3},
    {"Cr", 3.9, 3}
};

const std::vector<Base> BASES = {
    {"Cl", 21.0, 1},
    {"Br", 23.0, 1},
    {"F", 10.8, 1},
    {"I", 25.0, 1},
    {"NO₂", 10.6, 1},
    {"CN", 4.7, 1},
    {"CH₃COO", 9.25, 1},
    {"S", 1.0, 2},
    {"PO₄", 2.1, 3},
    {"CO₃", 3.6, 2},
    {"AsO₄", 2.5, 3}
};

// Helper function to map characters to superscript
char map_to_superscript(char c) {
    if (c >= '0' && c <= '9') {
        return SUPERSCRIPT_DIGITS[c - '0'];
    } else if (c == '-') {
        return SUPERSCRIPT_MINUS[0];
    } else if (c == '+') {
        return SUPERSCRIPT_PLUS[0];
    }
    return c;
}

// Helper function to map characters to subscript
char map_to_subscript(char c) {
    if (c >= '0' && c <= '9') {
        return SUBSCRIPT_DIGITS[c - '0'];
    }
    return c;
}

std::string superscripted(const std::string& str) {
    std::string result;
    for (char c : str) {
        result += map_to_superscript(c);
    }
    return result;
}

std::string subscripted(const std::string& str) {
    std::string result;
    for (char c : str) {
        result += map_to_subscript(c);
    }
    return result;
}

int count_uppers(const std::string& str) {
    int count = 0;
    for (char c : str) {
        if (std::isupper(static_cast<unsigned char>(c))) {
            count++;
        }
    }
    return count;
}

bool has_hydrogen(const std::string& molecule) {
    return molecule.find('H') != std::string::npos;
}

std::string create_valence_str(int valence) {
    if (valence > 9) {
        return ""; // Would need multi-digit support
    }
    return (valence == 1) ? "" : std::string(1, SUBSCRIPT_DIGITS[valence]);
}

bool str_is_float(const std::string& float_str) {
    if (float_str.empty()) return false;
    
    size_t dot_pos = float_str.find('.');
    
    // Case 1: No decimal point - must be all digits
    if (dot_pos == std::string::npos) {
        return !float_str.empty() && std::all_of(float_str.begin(), float_str.end(), 
            [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
    }
    
    // Case 2: Has decimal point
    std::string before_dot = float_str.substr(0, dot_pos);
    std::string after_dot = float_str.substr(dot_pos + 1);
    
    // Before dot can be empty (like .5) or all digits
    bool before_ok = before_dot.empty() || 
        std::all_of(before_dot.begin(), before_dot.end(), 
            [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
    
    // After dot must not be empty and all digits
    bool after_ok = !after_dot.empty() && 
        std::all_of(after_dot.begin(), after_dot.end(), 
            [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
    
    return before_ok && after_ok;
}