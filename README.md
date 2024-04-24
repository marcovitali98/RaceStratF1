# Race strategy F1 2024
Marco Vitale & Marco Selva

## How to run the code

### Makefile for Race Strategy F1

Use the following commands to manage the setup, execution, and cleanup of the virtual environment and application:

### Prerequisites

Ensure Python 3.x and Make are installed on your system. Check with:

    python3 --version
    make --version

Go to path '/python' to run the following commands.

### Commands

- **Setup**: Create the virtual environment, activate it, and install dependencies from `requirements.txt`.

        make setup

- **Run**: Execute the application script after ensuring the setup is complete.

        make run

- **Activate**: Manually activate the virtual environment.

        make activate

- **Lint**: Run the linter (`flake8`) on the source code to check for style and programming errors.

        make lint

- **Clean**: Remove the virtual environment and clean up temporary files like Python bytecode (`*.pyc`) and `__pycache__` directories.

        make clean

### Notes

- Update `requirements.txt` as needed to manage dependencies.
- If `make setup` fails, check permissions or consider running with `sudo` if necessary, although using `sudo` with `pip` should be done with caution to avoid potential system issues.
- Use `make lint` to ensure code quality before commits or deployments.

## Changelog

- 2024-04-24: Initial version

### Introduction

This document describes the strategy

## Tyre choice

## Fuel strategy

## Pit stops
