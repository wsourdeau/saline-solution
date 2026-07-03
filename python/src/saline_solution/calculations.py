"""
Chemical calculation functions for saline solution pH.
Refactored from saline_solution1.py to return data instead of printing.
"""

from typing import NamedTuple
from math import sqrt, log

from saline_solution.constants import PH_SCALE, Kw, WATER_MOLARITY, MSGS
from saline_solution.types import Salt, DominantReaction


class ReactionData(NamedTuple):
    """Data for a single reaction"""
    index: int
    description: str
    constant_name: str
    constant_value: float
    equation: str


class CalculationResults(NamedTuple):
    """Complete calculation results for a salt solution"""
    salt: Salt
    concentration: float
    dominant_reaction: DominantReaction
    pH: float
    pOH: float
    H3O: float
    OH: float
    inv_Kh: float
    x: float
    reactions: list[ReactionData]
    
    # Dominant reaction specific
    dr_H3O: float | None = None
    dr_OH: float | None = None
    
    # For display
    remaining_ion_concentration: float | None = None
    product_concentration: float | None = None


def calculate_results(salt: Salt, concentration: float) -> CalculationResults:
    """
    Calculate pH and other properties for a salt solution.
    Returns a data structure instead of printing.
    
    Args:
        salt: A Salt object containing acid and base
        concentration: Molar concentration of the salt solution
        
    Returns:
        CalculationResults with all calculated values
    """
    # Build reaction data for all 4 possible reactions
    reactions: list[ReactionData] = []
    
    # Reaction 1: Acid hydrolysis (cation + H2O)
    Ka = pow(10, -salt.acid.pKa)
    acid_charge = _get_ionic_charge_str("+", salt.acid.vAc)
    base2_formula = _get_conjugate_base(salt.acid)
    reaction1_eq = f"{salt.acid.name}{acid_charge} + H₂O → {base2_formula} + H₃O⁺"
    reactions.append(ReactionData(
        index=0,
        description="Acid Hydrolysis",
        constant_name="Ka",
        constant_value=Ka,
        equation=reaction1_eq
    ))
    
    # Reaction 2: Base hydrolysis (anion + H2O)
    Kb = pow(10, -salt.base.pKb)
    base_charge = _get_ionic_charge_str("-", salt.base.vBa)
    acid2_formula = _get_conjugate_acid(salt.base)
    reaction2_eq = f"{salt.base.name}{base_charge} + H₂O → {acid2_formula} + OH⁻"
    reactions.append(ReactionData(
        index=1,
        description="Base Hydrolysis",
        constant_name="Kb",
        constant_value=Kb,
        equation=reaction2_eq
    ))
    
    # Reaction 3: Salt hydrolysis (cation + anion + H2O)
    pKa2 = PH_SCALE - salt.base.pKb
    Ka2 = pow(10, -pKa2)
    inv_Kh = Ka / Ka2
    reaction3_eq = f"{salt.acid.name}{acid_charge} + {salt.base.name}{base_charge} (+H₂O) → {acid2_formula} + {base2_formula}"
    reactions.append(ReactionData(
        index=2,
        description="Salt Hydrolysis",
        constant_name="inv_Kh",
        constant_value=inv_Kh,
        equation=reaction3_eq
    ))
    
    # Reaction 4: Water autoionization
    reactions.append(ReactionData(
        index=3,
        description="Water Autoionization",
        constant_name="Kw",
        constant_value=Kw,
        equation="H₂O + H₂O → H₃O⁺ + OH⁻"
    ))
    
    # Determine dominant reaction
    K = Ka
    dr = 0
    if K < Kb:
        K = Kb
        dr = 1
    if K < inv_Kh:
        K = inv_Kh
        dr = 2
    if K < Kw:
        K = Kw
        dr = 3
    
    dominant_reaction = DominantReaction(dr)
    
    # Calculate based on dominant reaction
    H3O: float
    OH: float
    pH: float
    pOH: float
    dr_H3O: float | None = None
    dr_OH: float | None = None
    remaining_ion: float | None = None
    product: float | None = None
    
    match dr:
        case 0:  # Acid hydrolysis
            H3O = sqrt(Ka * concentration)
            OH = pow(10, -PH_SCALE) / H3O
            pH = -log(H3O) / log(10)
            pOH = -log(OH) / log(10)
            inv_Kh = Ka
            dr_H3O = H3O
            dr_OH = OH
            
            # Hydrolysis degree
            x = sqrt(Ka * concentration) / concentration
            remaining_ion = concentration - (x * concentration)
            product = x * concentration
            
        case 1:  # Base hydrolysis
            OH = sqrt(Kb * concentration)
            H3O = pow(10, -PH_SCALE) / OH
            pH = -log(H3O) / log(10)
            pOH = -log(OH) / log(10)
            inv_Kh = pow(10, -15.7) / pow(10, salt.base.pKb - PH_SCALE)
            dr_H3O = H3O
            dr_OH = OH
            
            x = sqrt(Kb * concentration) / concentration
            remaining_ion = concentration - (x * concentration)
            product = x * concentration
            
        case 2:  # Salt hydrolysis
            pH = (salt.acid.pKa + pKa2) / 2
            H3O = pow(10, -pH)
            pOH = PH_SCALE - pH
            OH = pow(10, -pOH)
            dr_H3O = H3O
            dr_OH = OH
            
            # For salt hydrolysis, x calculation needs review
            # Using simplified formula for now
            Kh = Kw / (Ka * Kb)
            x = sqrt(Kh * concentration) / concentration
            
        case 3:  # Neutral
            pH = 7
            H3O = pow(10, -7)
            pOH = pH
            OH = H3O
            inv_Kh = Kw
            x = 0  # No hydrolysis for neutral salts
    
    # Calculate x (hydrolysis degree) for non-neutral cases
    if dr != 3:
        # Use the hydrolysis degree based on dominant reaction
        if dr == 0:
            x = sqrt(Ka * concentration) / concentration
        elif dr == 1:
            x = sqrt(Kb * concentration) / concentration
        else:  # dr == 2
            Kh = Kw / (Ka * Kb)
            x = sqrt(Kh * concentration) / concentration
    else:
        x = 0
    
    return CalculationResults(
        salt=salt,
        concentration=concentration,
        dominant_reaction=dominant_reaction,
        pH=pH,
        pOH=pOH,
        H3O=H3O,
        OH=OH,
        inv_Kh=inv_Kh,
        x=x,
        reactions=reactions,
        dr_H3O=dr_H3O,
        dr_OH=dr_OH,
        remaining_ion_concentration=remaining_ion,
        product_concentration=product,
    )


def _get_conjugate_base(acid: "Acid") -> str:
    """Get the conjugate base formula for an acid"""
    from saline_solution.utils import has_hydrogen
    
    if has_hydrogen(acid.name):
        # For NH4+ -> NH3
        return f"{acid.name[0:-1]}{chr(ord(acid.name[-1]) - 1)}"
    else:
        # For metal cations: Ca2+ -> CaOH+
        return f"{acid.name}OH" + _get_ionic_charge_str("+", acid.vAc - 1)


def _get_conjugate_acid(base: "Base") -> str:
    """Get the conjugate acid formula for a base"""
    return f"H{base.name}" + _get_ionic_charge_str("-", base.vBa - 1)


def _get_ionic_charge_str(sign: str, valence: int) -> str:
    """Get superscript charge string"""
    from saline_solution.utils import superscripted
    
    if valence == 0:
        return ""
    charge_str = sign if valence == 1 else f"{valence}{sign}"
    return superscripted(charge_str)
