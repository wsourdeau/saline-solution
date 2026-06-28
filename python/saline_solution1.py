#!/usr/bin/python3

from collections.abc import Sequence, Mapping
from typing import Literal

from math import sqrt, log
from os import getenv

from saline_solution.constants import BASES, ACIDS, PH_SCALE, Kw, WATER_MOLARITY, MSGS
from saline_solution.types import Acid, Base, Salt
from saline_solution.utils import (
    beep,
    count_uppers,
    create_valence_str,
    has_hydrogen,
    str_is_float,
    subscripted,
    superscripted,
    HIDDEN_PARENS,
    VISIBLE_PARENS,
)


def _create_salt_formula(
    current_base: Base, grba: bool, current_acid: Acid, grac: bool
) -> str:
    open_paren_acid, close_paren_acid = (
        VISIBLE_PARENS if (current_base.vBa != 1 and grac) else HIDDEN_PARENS
    )
    open_paren_base, close_paren_base = (
        VISIBLE_PARENS if (current_acid.vAc != 1 and grba) else HIDDEN_PARENS
    )
    base_valence_str = create_valence_str(current_base.vBa)
    acid_valence_str = create_valence_str(current_acid.vAc)
    salt_formula = (
        f"{open_paren_acid}{current_acid.name}{close_paren_acid}{base_valence_str}"
        f"{open_paren_base}{current_base.name}{close_paren_base}{acid_valence_str}"
    )

    return salt_formula


def _build_salts(acids: Sequence[Acid], bases: Sequence[Base]) -> Mapping[str, Salt]:
    salts: dict[str, Salt] = {}

    for current_base in bases:
        grba = count_uppers(current_base.name) > 1

        for current_acid in acids:
            grac = count_uppers(current_acid.name) > 1

            salt_formula = _create_salt_formula(
                current_base,
                grba,
                current_acid,
                grac,
            )
            salts[salt_formula.lower()] = Salt(salt_formula, current_acid, current_base)

    return salts


def _get_concentration() -> float:
    while True:
        concentration_str = input("Quelle concentration : ")
        if str_is_float(concentration_str):
            result = float(concentration_str)
            if result > 0:
                return result
            print("Erreur: la concentration doit être positive")
        else:
            print("Erreur: veuillez entrer un nombre valide (ex: 0.1, 1.0)")


def _get_ionic_charge(sign: Literal["+", "-"], valence: int) -> str:
    if valence == 0:
        return ""

    ionic_charge_str = sign if valence == 1 else f"{valence}{sign}"
    result = superscripted(ionic_charge_str)

    return result


def _calculate(salt: Salt, concentration: float):
    """
    Calculate pH and other properties for a salt solution.

    Args:
        salt: A Salt
        concentration: Molar concentration of the salt solution

    Prints:
        Chemical reactions, constants, and final pH calculations
    """

    print(f"Solution saline : {salt.formula} (+H₂O) = ")
    acid2 = f"H{salt.base.name}{_get_ionic_charge('-', salt.base.vBa - 1)}"
    base2 = (
        f"{salt.acid.name[0:-1]}{chr(ord(salt.acid.name[-1]) - 1)}"
        if has_hydrogen(salt.acid.name)
        else f"{salt.acid.name}(OH)"
    ) + _get_ionic_charge("+", salt.acid.vAc - 1)

    Ka = pow(10, -salt.acid.pKa)
    print(
        f"1) Réaction  {salt.acid.name}{_get_ionic_charge('+', salt.acid.vAc)}  +  H₂O :"
        f"   ->  {base2}  +  H₃O⁺"
    )
    print(f"pKa = {salt.acid.pKa} Ka = {Ka}")
    print()

    Kb = pow(10, -salt.base.pKb)
    print(
        f"2) Réaction  {salt.base.name}{_get_ionic_charge('-', salt.base.vBa)}  +  H₂O :"
        f"   ->  {acid2}  +  OH⁻"
    )
    print(f"pKb = {salt.base.pKb} Kb = {Kb}")
    print()

    pKa2 = PH_SCALE - salt.base.pKb
    Ka2 = pow(10, -pKa2)
    inv_Kh = Ka / Ka2
    print(
        f"3) Réaction  {salt.acid.name}{_get_ionic_charge('+', salt.acid.vAc)}"
        f"  +  {salt.base.name}{_get_ionic_charge('-', salt.base.vBa)}  (+H₂O) :"
        f"   ->  {acid2}  +  {base2}"
    )
    print(f"inv_Kh = {inv_Kh}")
    print()

    print("4) Réaction  H₂O  +  H₂O :   ->  H₃O⁺  +  OH⁻")
    print(f"Kw = {Kw}")
    print()

    K = Ka
    dr = 0  # réaction déterminante
    if K < Kb:
        K = Kb
        dr = 1
    if K < inv_Kh:
        K = inv_Kh
        dr = 2
    if K < Kw:
        K = Kw
        dr = 3
    print(f"La réaction déterminante est la {dr + 1}º\nC'est un sel {MSGS[dr]}.")

    match dr:
        case 0:
            H3O = sqrt(Ka * concentration)
            OH = pow(10, -PH_SCALE) / H3O
            pH = -log(H3O) / log(10)
            pOH = -log(OH) / log(10)
            inv_Kh = Ka
            x = H3O / concentration
        case 1:
            OH = sqrt(Kb * concentration)
            H3O = pow(10, -PH_SCALE) / OH
            pH = -log(H3O) / log(10)
            pOH = -log(OH) / log(10)
            inv_Kh = pow(10, -15.7) / pow(10, salt.base.pKb - PH_SCALE)
            x = OH / concentration
        case 2:
            pH = (salt.acid.pKa + pKa2) / 2
            H3O = pow(10, -pH)
            pOH = PH_SCALE - pH
            OH = pow(10, -pOH)
            Kh = Kw / (Ka * Kb)  # Standard hydrolysis constant
            x = sqrt(Kh * concentration) / concentration
        case 3:
            pH = 7
            H3O = pow(10, -7)
            pOH = pH
            OH = H3O
            x = 1

    print("--------------------------")
    print(f"[H₃O⁺] = {H3O}  [OH⁻] = {OH}\npH = {pH}  pOH = {pOH}")

    if dr != 3:
        print(f"inv_Kh = {inv_Kh}")
        # x = (
        #     WATER_MOLARITY
        #     * inv_Kh
        #     * concentration
        #     / (WATER_MOLARITY * inv_Kh + inv_Kh * concentration)
        # )
        print(f"x = {x}")
        if dr == 0:
            ionic_charge_str = _get_ionic_charge("+", salt.acid.vAc)
            print(
                f"[{salt.acid.name}{ionic_charge_str}] = {concentration - x}    [{base2}] = {x}"
            )
        elif dr == 1:
            ionic_charge_str = _get_ionic_charge("-", salt.base.vBa)
            print(
                f"[{salt.base.name}{ionic_charge_str}] = {concentration - x}    [{acid2}] = {x}"
            )
    print("")


def _get_screen_width() -> int:
    env_columns = getenv("COLUMNS")
    screen_width = 80 if env_columns is None else int(env_columns)

    return screen_width


def _list_salts(salts: Mapping[str, Salt]):
    if len(salts) > 0:
        longest_salt = max(len(salt.formula) for salt in salts.values())

        screen_width = _get_screen_width()
        nbr_columns = screen_width // (longest_salt + 1)
        column_width = screen_width // nbr_columns

        for i, salt in enumerate(salts):
            len_salt = len(salt.formula)
            fixed_salt = (
                f"{salt.formula}{' ' * (column_width - len_salt)}"
                if len_salt <= column_width
                else salt.formula[0:column_width]
            )
            print(fixed_salt, end="")
            if (i % nbr_columns) == (nbr_columns - 1):
                print("")

    print("")


def _read_command() -> str:
    command = ""

    while len(command.strip()) == 0:
        command = input("Sel (ou commande) : ").strip()

    return command


def main():
    salts = _build_salts(ACIDS, BASES)

    print(
        "Programme de calcul des valeurs dans une solution saline.\n"
        "Écrit en Novembre 1994 par Wolfgang Sourdeau\n"
        "\n"
        "Commandes possibles :\n"
        "L -> liste des sels\n"
        "F -> fin\n"
    )

    while True:
        try:
            command = _read_command()
        except EOFError:
            print()
            break

        if len(command) == 1:
            match command[0]:
                case "L" | "l":
                    _list_salts(salts)
                case "F" | "f":
                    break
                case _:
                    beep()
        else:
            salt_name = subscripted(command)
            if (salt := salts.get(salt_name)) is None:
                print("Sel non trouvé")
            else:
                concentration = _get_concentration()
                _calculate(salt, concentration)


if __name__ == "__main__":
    main()
