from collections.abc import Sequence
from .types import Acid, Base

WATER_MOLARITY = 55.5  # mol/L at 25°C
PH_SCALE = 14.0
Kw = 1e-14

MSGS: Sequence[str] = [
    "à réaction acide/d'acide fort et de base faible",
    "à réaction basique/d'acide faible et de base forte",
    "d'acide faible et de base faible",
    "neutre/d'acide fort et de base forte",
]

ACIDS: Sequence[Acid] = [
    Acid(name, pKa, vAc)
    for (name, pKa, vAc) in [
        ("K", 15.6, 1),
        ("Na", 14.8, 1),
        ("NH₄", 9.2, 1),
        ("Ca", 12.7, 2),
        ("Pb", 8.8, 2),
        ("Cd", 8.5, 2),
        ("Cu", 8, 2),
        ("Zn", 7.9, 2),
        ("Sn", 1.7, 2),
        ("Al", 4, 3),
        ("Cr", 3.9, 3),
    ]
]

BASES: Sequence[Base] = [
    Base(name, pKb, vBa)
    for (name, pKb, vBa) in [
        ("Cl", 21, 1),
        ("Br", 23, 1),
        ("F", 10.8, 1),
        ("I", 25, 1),
        ("NO₂", 10.6, 1),
        ("CN", 4.7, 1),
        ("CH₃COO", 9.25, 1),
        ("S", 1, 2),
        ("PO₄", 2.1, 3),
        ("CO₃", 3.6, 2),
        ("AsO₄", 2.5, 3),
    ]
]
