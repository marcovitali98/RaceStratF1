# Make file for Race Strategy F1

# Name of the virtual environment
VENV = .venv

# Python version
PYTHON = python3

# Default command
all: setup

# Setup the Python virtual environment and install dependencies
setup: $(VENV)/bin/activate

$(VENV)/bin/activate: requirements.txt
	@echo "Checking and setting up virtual environment..."
	@$(PYTHON) -m venv $(VENV) 2>/dev/null || (echo "Failed to create venv, attempting to install python3-venv package..."; sudo apt-get install $(PYTHON)-venv)
	@echo "Virtual environment created successfully."
	@$(VENV)/bin/pip install -r requirements.txt
	@touch $(VENV)/bin/activate
	@echo "Setup complete. Virtual environment is ready."

# Command to run the application
run: setup
	@echo "Running the application..."
	@$(VENV)/bin/python src/main.py

# Clean the project
clean:
	@echo "Cleaning up the project..."
	@rm -rf $(VENV)
	@find src/ -type f -name '*.pyc' -delete
	@find src/ -type d -name '__pycache__' -delete
	@echo "Clean up complete."

.PHONY: all setup run clean
