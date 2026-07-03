from dataclasses import dataclass
from enum import IntEnum


class DominantReaction(IntEnum):
    ACID_HYDROLYSIS = 0
    BASE_HYDROLYSIS = 1
    SALT_HYDROLYSIS = 2
    NEUTRAL = 3


@dataclass
class Acid:
    name: str
    pKa: float
    vAc: int


@dataclass
class Base:
    name: str
    pKb: float
    vBa: int


@dataclass
class Salt:
    formula: str
    acid: Acid
    base: Base
