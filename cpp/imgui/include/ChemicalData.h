#pragma once

#include <string>
#include <vector>
#include <optional>

// Superscript characters for chemical notation
const std::string SUPERSCRIPT_DIGITS = "⁰¹²³⁴⁵⁶⁷⁸⁹";
const std::string SUPERSCRIPT_MINUS = "⁻";
const std::string SUPERSCRIPT_PLUS = "⁺";

// Subscript characters for chemical notation
const std::string SUBSCRIPT_DIGITS = "₀₁₂₃₄₅₆₇₈₉";

// Chemical constants
const double WATER_MOLARITY = 55.5; // mol/L at 25°C
const double PH_SCALE = 14.0;
const double Kw = 1e-14; // Water ion product constant

// Messages for dominant reactions (same as Python version)
const std::vector<std::string> MSGS = {
    "reaction acide/d'acide fort et de base faible",
    "reaction basique/d'acide faible et de base forte",
    "d'acide faible et de base faible",
    "neutre/d'acide fort et de base forte"
};

// Parenthesis constants
const std::pair<std::string, std::string> HIDDEN_PARENS = {"", ""};
const std::pair<std::string, std::string> VISIBLE_PARENS = {"(", ")"};

// Enums
enum class DominantReaction {
    ACID_HYDROLYSIS = 0,
    BASE_HYDROLYSIS = 1,
    SALT_HYDROLYSIS = 2,
    NEUTRAL = 3
};

// Data structures
struct Acid {
    std::string name;
    double pKa;
    int vAc; // Valence of acid (charge)

    Acid(const std::string& name, double pKa, int vAc) 
        : name(name), pKa(pKa), vAc(vAc) {}
};

struct Base {
    std::string name;
    double pKb;
    int vBa; // Valence of base (charge)

    Base(const std::string& name, double pKb, int vBa) 
        : name(name), pKb(pKb), vBa(vBa) {}
};

struct Salt {
    std::string formula;
    Acid acid;
    Base base;

    Salt(const std::string& formula, const Acid& acid, const Base& base) 
        : formula(formula), acid(acid), base(base) {}
};

// Reaction data
struct ReactionData {
    int index;
    std::string description;
    std::string constant_name;
    double constant_value;
    std::string equation;

    ReactionData(int index, const std::string& description, 
                const std::string& constant_name, double constant_value,
                const std::string& equation)
        : index(index), description(description), constant_name(constant_name),
          constant_value(constant_value), equation(equation) {}
};

// Complete calculation results
struct CalculationResults {
    Salt salt;
    double concentration;
    DominantReaction dominant_reaction;
    double pH;
    double pOH;
    double H3O; // [H3O+] concentration
    double OH;  // [OH-] concentration
    double inv_Kh; // Inverse hydrolysis constant
    double x;     // Hydrolysis degree

    std::vector<ReactionData> reactions;

    // Dominant reaction specific
    std::optional<double> dr_H3O;
    std::optional<double> dr_OH;

    // For display
    std::optional<double> remaining_ion_concentration;
    std::optional<double> product_concentration;

    CalculationResults(const Salt& salt, double concentration, DominantReaction dominant_reaction,
                       double pH, double pOH, double H3O, double OH, double inv_Kh, double x,
                       const std::vector<ReactionData>& reactions,
                       std::optional<double> dr_H3O = std::nullopt,
                       std::optional<double> dr_OH = std::nullopt,
                       std::optional<double> remaining_ion_concentration = std::nullopt,
                       std::optional<double> product_concentration = std::nullopt)
        : salt(salt), concentration(concentration), dominant_reaction(dominant_reaction),
          pH(pH), pOH(pOH), H3O(H3O), OH(OH), inv_Kh(inv_Kh), x(x),
          reactions(reactions), dr_H3O(dr_H3O), dr_OH(dr_OH),
          remaining_ion_concentration(remaining_ion_concentration),
          product_concentration(product_concentration) {}
};

// Global chemical data
extern const std::vector<Acid> ACIDS;
extern const std::vector<Base> BASES;

// Utility functions
std::string superscripted(const std::string& str);
std::string subscripted(const std::string& str);
int count_uppers(const std::string& str);
bool has_hydrogen(const std::string& molecule);
std::string create_valence_str(int valence);
bool str_is_float(const std::string& float_str);
