def tui_main():
    from .tui_app import SalineSolutionApp

    app = SalineSolutionApp()
    app.run()


def cli_main():
    from .cli_app import cli_app_main

    cli_app_main()
