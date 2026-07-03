from textual.app import App, ComposeResult
from textual.containers import Container, Horizontal, Vertical
from textual.widgets import (
    Header,
    ListView,
    ListItem,
    Static,
    Input,
    Button,
    Label,
    Markdown,
    Switch,
)
from textual.binding import Binding
from textual.message import Message

from .calculations import (
    calculate_results,
    CalculationResults,
)

from .constants import ACIDS, BASES, MSGS
from .types import Acid, Base, Salt
from .utils import (
    superscripted,
    has_hydrogen,
    count_uppers,
    create_valence_str,
    str_is_float,
    HIDDEN_PARENS,
    VISIBLE_PARENS,
)


class AcidSelected(Message):
    """Message sent when an acid is selected"""

    def __init__(self, acid: Acid):
        self.acid = acid
        super().__init__()


class BaseSelected(Message):
    """Message sent when a base is selected"""

    def __init__(self, base: Base):
        self.base = base
        super().__init__()


class AcidList(ListView):
    """Left pane: List of acids"""

    selected_acid: Acid | None = None

    def compose(self) -> ComposeResult:
        self.border_title = "[bold red]Acids[/bold red]"

        for acid in ACIDS:
            yield ListItem(Label(acid.name))

    def on_list_view_selected(self, event: ListView.Selected) -> None:
        if 0 <= event.index < len(ACIDS):
            self.selected_acid = ACIDS[event.index]
            self.post_message(AcidSelected(self.selected_acid))


class BaseList(ListView):
    """Right pane: List of bases"""

    selected_base: Base | None = None

    def compose(self) -> ComposeResult:
        self.border_title = "[bold blue]Base[/bold blue]"

        for base in BASES:
            yield ListItem(Label(base.name))

    def on_list_view_selected(self, event: ListView.Selected) -> None:
        if 0 <= event.index < len(BASES):
            self.selected_base = BASES[event.index]
            self.post_message(BaseSelected(self.selected_base))


class SaltDisplay(Static):
    """Middle: Shows the composed salt formula"""

    def compose(self) -> ComposeResult:
        with Vertical(id="concentration-entry"):
            yield Label("Salt:")

            # Input
            yield Input(placeholder="Concentration (M)", id="concentration-input")

            # Button
            yield Button("Calculate pH", id="calculate-button")


def _selected_str(value: str) -> str:
    return f"[$accent] {value} [/$accent]"


class ResultsPane(Container):
    """Bottom pane: Shows calculation results"""

    results: CalculationResults | None = None
    show_details: bool = False

    def compose(self) -> ComposeResult:
        self.border_title = "[bold cyan]Calculation Results[/bold cyan]"
        with Horizontal(id="results-buttons"):
            no_label, yes_label = (
                ("no", _selected_str("yes"))
                if self.show_details
                else (_selected_str("no"), " yes ")
            )
            yield Label(f"More details: {no_label}")
            disabled = self.results is None
            yield Switch(id="more-details-switch", disabled=disabled)
            yield Label(f" {yes_label}")
        yield Markdown(id="results-markdown")

    def clear(self) -> None:
        self.show(None)

    def show(self, results: CalculationResults | None) -> None:
        self.results = results
        self.show_details = False
        self._update_display()

    def toggle_details(self) -> None:
        self.show_details = not self.show_details
        self._update_display()

    def _update_display(self) -> None:
        more_details_switch: Switch = self.query_one("#more-details-switch", Switch)
        more_details_switch.value = self.results is not None and self.show_details
        more_details_switch.disabled = self.results is None

        markdown_widget = self.query_one("#results-markdown", Markdown)
        markdown = (
            self._format_full_results() if self.show_details else self._format_summary()
        )
        markdown_widget.update(markdown)

    def _format_summary(self) -> str:
        """Format a concise summary of results"""
        r = self.results
        if r is None:
            return ""

        dr_index = r.dominant_reaction.value

        return f"""## Dominant Reaction: {MSGS[dr_index]}

| Property | Value |
|----------|-------|
| pH | {r.pH:.4f} |
| pOH | {r.pOH:.4f} |
| [H₃O⁺] | {r.H3O:.2e} M |
| [OH⁻] | {r.OH:.2e} M |
| Hydrolysis degree (x) | {r.x:.4f} |
"""

    def _format_full_results(self) -> str:
        """Format complete results with all 4 reactions"""
        r = self.results
        if r is None:
            return ""

        dr_index = r.dominant_reaction.value

        md = f"""
## Dominant Reaction: {MSGS[dr_index]}

### Final Results

| Property | Value |
|----------|-------|
| pH | {r.pH:.4f} |
| pOH | {r.pOH:.4f} |
| [H₃O⁺] | {r.H3O:.2e} M |
| [OH⁻] | {r.OH:.2e} M |
| inv_Kh | {r.inv_Kh:.2e} |
| Hydrolysis degree (x) | {r.x:.4f} |

---

### All Reactions

"""

        for rxn in r.reactions:
            is_dominant = rxn.index == dr_index
            marker = "🟢" if is_dominant else "⚪"
            md += f"\n{marker} **Reaction {rxn.index + 1}: {rxn.description}**\n"
            md += f"   - Equation: {rxn.equation}\n"
            md += f"   - Constant ({rxn.constant_name}): {rxn.constant_value:.2e}\n"

        # Add dominant reaction specifics
        tmp = """
---

### Dominant Reaction Details

"""

        if r.remaining_ion_concentration is not None:
            acid_charge = create_valence_str(r.salt.acid.vAc)
            md += f"- [{r.salt.acid.name}{acid_charge}] = {r.remaining_ion_concentration:.4g} M\n"
            md += f"- [{_get_conjugate_base(r.salt.acid)}] = {r.product_concentration:.4g} M\n"

        if r.dr_H3O is not None:
            md += f"- [H₃O⁺] = {r.dr_H3O:.2e} M\n"
        if r.dr_OH is not None:
            md += f"- [OH⁻] = {r.dr_OH:.2e} M\n"

        return md


class SalineSolutionApp(App):
    """Main Textual application for saline solution calculator"""

    CSS_PATH = "saline_solution.tcss"

    BINDINGS = [
        Binding("q", "quit", "Quit"),
        Binding("escape", "clear_selection", "Clear Selection"),
        Binding("c", "calculate", "Calculate"),
    ]

    def __init__(self):
        super().__init__()
        self.selected_acid: Acid | None = None
        self.selected_base: Base | None = None
        self.title = "⚗️  Saline Solution"

    def _create_salt_formula(self, current_base: Base, current_acid: Acid) -> str:
        grac = count_uppers(current_acid.name) > 1
        open_paren_acid, close_paren_acid = (
            VISIBLE_PARENS if (current_base.vBa != 1 and grac) else HIDDEN_PARENS
        )
        grba = count_uppers(current_base.name) > 1
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

    def compose(self) -> ComposeResult:
        yield Header()
        with Vertical():
            # Top row: acids, salt display, bases
            with Horizontal(id="elements-entry"):
                yield AcidList(id="acid-list")
                yield SaltDisplay(id="salt-display")
                yield BaseList(id="base-list")

            # Results
            rp = ResultsPane(id="results-pane")
            yield rp

    def on_ready(self) -> None:
        self.update_salt_display()

    def on_acid_selected(self, message: AcidSelected) -> None:
        self.selected_acid = message.acid
        self.update_salt_display()
        self.clear_results()

    def on_base_selected(self, message: BaseSelected) -> None:
        self.selected_base = message.base
        self.update_salt_display()
        self.clear_results()

    def update_salt_display(self) -> None:
        """Update the salt display when acid or base is selected"""
        salt_display: SaltDisplay = self.query_one("#salt-display", SaltDisplay)
        calculate_btn: Button = self.query_one("#calculate-button", Button)

        calculate_btn.disabled = True
        if self.selected_acid:
            if self.selected_base:
                salt_formula = self._create_salt_formula(
                    self.selected_base, self.selected_acid
                )
                salt_formula_label = f"[bold green]{salt_formula}[/bold green]"
                calculate_btn.disabled = False
            else:
                salt_formula_label = (
                    f"[bold yellow]{self.selected_acid.name} + ...[/bold yellow]"
                )
        elif self.selected_base:
            salt_formula_label = (
                f"[bold yellow]... + {self.selected_base.name}[/bold yellow]"
            )
        else:
            salt_formula_label = "[dim]Select one acid and one base[/dim]"
        salt_display.update(salt_formula_label)

    def clear_results(self) -> None:
        """Hide the results pane"""
        results_pane: ResultsPane = self.query_one("#results-pane", ResultsPane)
        results_pane.clear()

    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "calculate-button":
            self.calculate()
        elif event.button.id == "close-button":
            self.clear_results()

    def on_switch_changed(self, event: Switch.Changed) -> None:
        if event.switch.id == "more-details-switch":
            self.query_one("#results-pane", ResultsPane).toggle_details()

    def on_input_changed(self, event: Input.Changed) -> None:
        """Hide results when concentration changes"""
        if event.input.id == "concentration-input":
            self.clear_results()

    def calculate(self) -> None:
        """Perform the calculation and show results"""
        if not self.selected_acid or not self.selected_base:
            self.notify("Please select both an acid and a base", severity="error")
            return

        # Create salt from selected acid and base
        formula = self._create_salt_formula(self.selected_base, self.selected_acid)
        salt = Salt(formula, self.selected_acid, self.selected_base)

        # Get concentration
        concentration_input = self.query_one("#concentration-input", Input)
        concentration_str = concentration_input.value
        if str_is_float(concentration_str):
            concentration = float(concentration_str or "0.1")
            if concentration > 0:
                # Calculate
                results = calculate_results(salt, concentration)

                # Show results
                results_pane = self.query_one("#results-pane", ResultsPane)
                results_pane.show(
                    results
                )  # Starts with summary view (show_details=False)
            else:
                self.notify(
                    "Invalid concentration. Please enter a positive number.",
                    severity="error",
                )
        else:
            self.notify(
                "Invalid concentration. Please enter a float number.",
                severity="error",
            )

    def action_clear_selection(self) -> None:
        """Clear acid and base selection"""
        acid_list = self.query_one("#acid-list", AcidList)
        base_list = self.query_one("#base-list", BaseList)

        acid_list.selected_acid = None
        base_list.selected_base = None
        self.selected_acid = None
        self.selected_base = None
        self.update_salt_display()
        self.clear_results()

    def action_calculate(self) -> None:
        """Trigger calculation via keyboard"""
        self.calculate()


def _get_conjugate_base(acid: Acid) -> str:
    """Get the conjugate base formula for an acid"""
    if has_hydrogen(acid.name):
        # For NH4+ -> NH3
        return f"{acid.name[0:-1]}{chr(ord(acid.name[-1]) - 1)}"

    # For metal cations: Ca2+ -> CaOH+
    charge = acid.vAc - 1
    match charge:
        case 0:
            charge_str = ""
        case 1:
            charge_str = superscripted("+")
        case _:
            charge_str = superscripted(f"{charge}+")

    return f"{acid.name}OH{charge_str}"
