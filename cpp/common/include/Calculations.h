#pragma once

#include <memory>

#include "ChemicalData.h"

// Function declarations for chemical calculations
std::unique_ptr<CalculationResults> calculate_results(const Salt& salt, double concentration);

// Helper functions (internal to calculations)
std::string get_conjugate_base(const Acid& acid);
std::string get_conjugate_acid(const Base& base);
std::string get_ionic_charge_str(char sign, int valence);
