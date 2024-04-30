# Make file for Race Strategy F1

# Name of the virtual environment
VENV = .venv

# Python version
PYTHON = python3

# Default command
all: setup

install:
	@echo "Installing dependencies..."
	@$(PYTHON) -m venv $(VENV)
	@echo "Dependencies installed."

# Setup and install the Python virtual environment and install dependencies
setup: $(VENV)/bin/activate
	@echo "Setting up the project..."
	@$(VENV)/bin/pip install -r requirements.txt
	@echo "Setup complete."

# Command to run the application
run: setup
	@echo "Running the application..."
	@$(VENV)/bin/python src/main.py

activate:
	@echo "Activating virtual environment..."
	@bash -c "source .venv/bin/activate"

# linter with flake8
lint:
	@echo "Running linter..."
	@bash -c "source .venv/bin/activate && flake8 src/"
	@echo "Linter complete."

# Clean the project
clean:
	@echo "Cleaning up the project..."
	@rm -rf $(VENV)
	@find src/ -type f -name '*.pyc' -delete
	@find src/ -type d -name '__pycache__' -delete
	@echo "Clean up complete."

.PHONY: all setup run activate lint clean
