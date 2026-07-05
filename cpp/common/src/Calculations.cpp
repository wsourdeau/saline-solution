#include <cmath>
#include <vector>
#include <memory>

#include "Calculations.h"


std::string get_conjugate_base(const Acid& acid) {
    if (has_hydrogen(acid.name)) {
        // For NH4+ -> NH3 (special case: remove + and change 4 to 3)
        if (acid.name == "NH₄") {
            return "NH₃";
        }
        // For other hydrogen-containing acids: remove last H and decrease subscript
        // This is a simplified version - in the original Python code it was:
        // return f"{acid.name[0:-1]}{chr(ord(acid.name[-1]) - 1)}"
        // Which works for NH4+ -> NH3 but not for general cases
        if (!acid.name.empty()) {
            return acid.name.substr(0, acid.name.size() - 1);
        }
    }

    // For metal cations: Ca2+ -> CaOH+
    int charge = acid.vAc - 1;
    std::string charge_str = get_ionic_charge_str('+', charge);
    return acid.name + "OH" + charge_str;
}

std::string get_conjugate_acid(const Base& base) {
    int charge = base.vBa - 1;
    std::string charge_str = get_ionic_charge_str('-', charge);
    return "H" + base.name + charge_str;
}

std::string get_ionic_charge_str(char sign, int valence) {
    if (valence == 0) {
        return "";
    }
    std::string charge_str;
    if (valence == 1) {
        charge_str = std::string(1, sign);
    } else {
        charge_str = std::to_string(valence) + sign;
    }
    return superscripted(charge_str);
}

std::unique_ptr<CalculationResults> calculate_results(const Salt& salt, double concentration) {
    // Build reaction data for all 4 possible reactions
    std::vector<ReactionData> reactions;

    // Reaction 1: Acid hydrolysis (cation + H2O)
    double Ka = std::pow(10.0, -salt.acid.pKa);
    std::string acid_charge = get_ionic_charge_str('+', salt.acid.vAc);
    std::string base2_formula = get_conjugate_base(salt.acid);
    std::string reaction1_eq = salt.acid.name + acid_charge + " + H₂O → " + base2_formula + " + H₃O⁺";
    reactions.emplace_back(
        0, "Acid Hydrolysis", "Ka", Ka, reaction1_eq
    );

    // Reaction 2: Base hydrolysis (anion + H2O)
    double Kb = std::pow(10.0, -salt.base.pKb);
    std::string base_charge = get_ionic_charge_str('-', salt.base.vBa);
    std::string acid2_formula = get_conjugate_acid(salt.base);
    std::string reaction2_eq = salt.base.name + base_charge + " + H₂O → " + acid2_formula + " + OH⁻";
    reactions.emplace_back(
        1, "Base Hydrolysis", "Kb", Kb, reaction2_eq
    );

    // Reaction 3: Salt hydrolysis (cation + anion + H2O)
    double pKa2 = PH_SCALE - salt.base.pKb;
    double Ka2 = std::pow(10.0, -pKa2);
    double inv_Kh = Ka / Ka2;
    std::string reaction3_eq = salt.acid.name + acid_charge + " + " + salt.base.name + base_charge + " (+H₂O) → " + acid2_formula + " + " + base2_formula;
    reactions.emplace_back(
        2, "Salt Hydrolysis", "inv_Kh", inv_Kh, reaction3_eq
    );

    // Reaction 4: Water autoionization
    reactions.emplace_back(
        3, "Water Autoionization", "Kw", Kw, "H₂O + H₂O → H₃O⁺ + OH⁻"
    );

    // Determine dominant reaction
    double K = Ka;
    int dr = 0; // Default to acid hydrolysis
    if (K < Kb) {
        K = Kb;
        dr = 1; // Base hydrolysis
    }
    if (K < inv_Kh) {
        K = inv_Kh;
        dr = 2; // Salt hydrolysis
    }
    if (K < Kw) {
        K = Kw;
        dr = 3; // Neutral
    }

    DominantReaction dominant_reaction = static_cast<DominantReaction>(dr);

    // Calculate based on dominant reaction
    double H3O = 0.0;
    double OH = 0.0;
    double pH = 0.0;
    double pOH = 0.0;
    double final_inv_Kh = inv_Kh;
    std::optional<double> dr_H3O = std::nullopt;
    std::optional<double> dr_OH = std::nullopt;
    std::optional<double> remaining_ion = std::nullopt;
    std::optional<double> product = std::nullopt;
    double x = 0.0;

    switch (dr) {
        case 0: { // Acid hydrolysis
            H3O = std::sqrt(Ka * concentration);
            OH = std::pow(10.0, -PH_SCALE) / H3O;
            pH = -std::log10(H3O);
            pOH = -std::log10(OH);
            final_inv_Kh = Ka;
            dr_H3O = H3O;
            dr_OH = OH;

            // Hydrolysis degree
            x = std::sqrt(Ka * concentration) / concentration;
            remaining_ion = concentration - (x * concentration);
            product = x * concentration;
            break;
        }
        case 1: { // Base hydrolysis
            OH = std::sqrt(Kb * concentration);
            H3O = std::pow(10.0, -PH_SCALE) / OH;
            pH = -std::log10(H3O);
            pOH = -std::log10(OH);
            final_inv_Kh = std::pow(10.0, -15.7) / std::pow(10.0, salt.base.pKb - PH_SCALE);
            dr_H3O = H3O;
            dr_OH = OH;

            x = std::sqrt(Kb * concentration) / concentration;
            remaining_ion = concentration - (x * concentration);
            product = x * concentration;
            break;
        }
        case 2: { // Salt hydrolysis
            pH = (salt.acid.pKa + pKa2) / 2.0;
            H3O = std::pow(10.0, -pH);
            pOH = PH_SCALE - pH;
            OH = std::pow(10.0, -pOH);
            dr_H3O = H3O;
            dr_OH = OH;

            // For salt hydrolysis, x calculation
            double Kh = Kw / (Ka * Kb);
            x = std::sqrt(Kh * concentration) / concentration;
            break;
        }
        case 3: { // Neutral
            pH = 7.0;
            H3O = std::pow(10.0, -7.0);
            pOH = pH;
            OH = H3O;
            final_inv_Kh = Kw;
            x = 0.0; // No hydrolysis for neutral salts
            break;
        }
    }

    return std::make_unique<CalculationResults>(
        salt, concentration, dominant_reaction,
        pH, pOH, H3O, OH, final_inv_Kh, x,
        reactions, dr_H3O, dr_OH, remaining_ion, product
    );
}
