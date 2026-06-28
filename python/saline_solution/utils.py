from collections.abc import Sequence, Mapping

HIDDEN_PARENS = ("", "")
VISIBLE_PARENS = ("(", ")")

# sub-/superscript mapping
SUBSCRIPT = ["₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉"]
SUPERSCRIPT = ["⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"]

_SUBSCRIPT_MAP = {}
_SUPERSCRIPT_MAP = {"-": "⁻", "+": "⁺"}


def count_uppers(string: str) -> int:
    """Count the number of upper characters in the string"""
    return sum(1 for c in string if c.isupper())


def has_hydrogen(molecule: str) -> bool:
    """Determine whether the given molecule has at least 1 hydrogen atom"""
    return molecule.find("H") != -1


def beep():
    """Emits a beep sound via the terminal."""
    print(chr(7))


def _adjustscript(
    string: str, digits: Sequence[str], mapping: Mapping[str, str]
) -> str:
    def get_part(char: str) -> str:
        if char.isdigit():
            digit = int(char)
            return digits[digit]
        if (adjusted_char := mapping.get(char)) is not None:
            return adjusted_char
        return char

    parts: list[str] = [get_part(char) for char in string]
    result = "".join(parts)

    return result


def subscripted(string: str) -> str:
    """Return a string where all subscriptable characters have been converted into
    unicode subscripted equivalents."""
    return _adjustscript(string, SUBSCRIPT, _SUBSCRIPT_MAP)


def superscripted(string: str) -> str:
    """Return a string where all superscriptable characters have been converted into
    unicode superscripted equivalents."""
    return _adjustscript(string, SUPERSCRIPT, _SUPERSCRIPT_MAP)


def str_is_float(float_str: str) -> bool:
    """Determine whether `float_str` represent a valid float character."""
    if float_str.isdigit():
        return True

    parts = float_str.split(".")
    result = (
        len(parts) == 2
        and (len(parts[0]) == 0 or parts[0].isdigit())
        and parts[1].isdigit()
    )

    return result


def create_valence_str(valence: int) -> str:
    """Return a valence represented as a string with subscribed value."""

    if valence > 9:
        raise ValueError("SUBSCRIPT would not support this value")
    return "" if (valence == 1) else f"{SUBSCRIPT[valence]}"
