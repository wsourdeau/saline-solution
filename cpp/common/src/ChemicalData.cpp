#include <algorithm>
#include <cctype>
#include <string>
#include <stdexcept>

#include "ChemicalData.h"


using namespace std;

// Superscript characters for chemical notation
const array<string, 10> SUPERSCRIPT_DIGITS{"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};
const string SUPERSCRIPT_MINUS("⁻");
const string SUPERSCRIPT_PLUS("⁺");

// Subscript characters for chemical notation
const array<string, 10> SUBSCRIPT_DIGITS{"₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉"};

// Chemical constants
const double WATER_MOLARITY = 55.5; // mol/L at 25°C
const double PH_SCALE = 14.0;
const double Kw = 1e-14; // Water ion product constant

// Messages for dominant reactions (same as Python version)
const vector<string> MSGS{
    "reaction acide/d'acide fort et de base faible",
    "reaction basique/d'acide faible et de base forte",
    "d'acide faible et de base faible",
    "neutre/d'acide fort et de base forte"
};

// Parenthesis constants
const pair<string, string> HIDDEN_PARENS{"", ""};
const pair<string, string> VISIBLE_PARENS{"(", ")"};

// Global chemical data initialization
const vector<Acid> ACIDS{
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

const vector<Base> BASES{
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

namespace {

inline bool has_superscript(char c)
{
    return ((c >= '0' && c <= '9') || (c == '-') || (c == '+'));
}
  
        
// Helper function to map characters to superscript
const string & map_to_superscript(char c)
{
    if (c >= '0' && c <= '9')
        return SUPERSCRIPT_DIGITS[c - '0'];
    if (c == '-')
        return SUPERSCRIPT_MINUS;
    if (c == '+')
        return SUPERSCRIPT_PLUS;

    throw invalid_argument("Superscript character not found");
}

inline bool has_subscript(char c)
{
    return (c >= '0' && c <= '9');
}

const string & map_to_subscript(char c)
{
    if (c >= '0' && c <= '9')
        return SUBSCRIPT_DIGITS[c - '0'];

    throw invalid_argument("Subscript character not found");
}

} // local file namespace


string superscripted(const string & str)
{
    string result;

    for (char c : str) {
        if (has_superscript(c))
            result += map_to_superscript(c);
        else
            result += c;
    }

    return result;
}

string subscripted(const string & str)
{
    string result;

    for (char c : str) {
      if (has_subscript(c))
        result += map_to_subscript(c);
      else
	result += c;
    }

    return result;
}

int count_uppers(const string & str)
{
    int count = 0;

    for (char c : str) {
        if (isupper(static_cast<unsigned char>(c)))
            count++;
    }

    return count;
}

bool has_hydrogen(const string & molecule)
{
    return molecule.find('H') != string::npos;
}

string create_valence_str(int valence)
{
    string valence_str;
  
    if (valence > 1) {
        string ascii_valence_str = to_string(valence);
        valence_str =  subscripted(ascii_valence_str);
    }

  return valence_str;
}

bool str_is_float(const string & float_str)
{
    if (float_str.empty()) return false;
    
    size_t dot_pos = float_str.find('.');
    
    // Case 1: No decimal point - must be all digits
    if (dot_pos == string::npos)
        return all_of(float_str.begin(), float_str.end(), 
		      [](char c) { return isdigit(static_cast<unsigned char>(c)); });
    
    // Case 2: Has decimal point
    string before_dot = float_str.substr(0, dot_pos);
    string after_dot = float_str.substr(dot_pos + 1);
    
    // Before dot can be empty (like .5) or all digits
    bool before_ok = (before_dot.empty() ||
		      all_of(before_dot.begin(), before_dot.end(), 
		             [](char c) { return isdigit(static_cast<unsigned char>(c)); }));
    
    // After dot must not be empty and all digits
    bool after_ok = (!after_dot.empty()
		     && all_of(after_dot.begin(), after_dot.end(), 
			       [](char c) { return isdigit(static_cast<unsigned char>(c)); }));
    
    return before_ok && after_ok;
}
