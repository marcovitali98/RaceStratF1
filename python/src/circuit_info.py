import json

from loguru import logger
# File path to your JSON data
file_path = '../circuit.json'


class CircuitInfo():

    def __init__(self, file_path: str, logger: logger):
        self.data = None
        self.file_path = file_path
        self.logger = logger
        self.circuit_info = None
        self.get_file()
    
    def get_file(self):
        try:
            with open(self.file_path, 'r') as file:
                self.data = json.load(file)
        except FileNotFoundError:
            return "File not found"
        except json.JSONDecodeError:
            return "Error decoding JSON"
        except Exception as e:
            return logger.exception(e)
        
    def get_available_circuits(self):
        try:
            # retrunt the available circuits, field circuit_name
            circuits = self.data.keys()
            print(circuits)
            return circuits
        except Exception as e:
            return logger.exception(e)


    def get_circuit_info(self, circuit_name):
        try:
            self.circuit_info = self.data[circuit_name]
            return self.circuit_info
        except Exception as e:
            return str(e)

    def user_input(self):

        # show the available circuits to the user
        for circuit in self.get_available_circuits():
            self.logger.info(circuit)

        # get the circuit name from the user
        circuit_name = input("Enter the circuit name: ")

        # Usage
        circuit_info = self.get_circuit_info(circuit_name)
        return circuit_info
