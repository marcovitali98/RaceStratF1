import json
# File path to your JSON data
file_path = '../circuit.json'


from loguru import logger



class CircuitInfo():
    def get_circuit_info_from_file(self, file_path, circuit_name):
        try:
            with open(file_path, 'r') as file:
                data = json.load(file)
                circuit_info = data.get(circuit_name, "Circuit not found")
                return circuit_info
        except FileNotFoundError:
            return "File not found"
        except json.JSONDecodeError:
            return "Error decoding JSON"
        except Exception as e:
            return str(e)
    
    # show the available circuits to the user
    def show_available_circuits(self, file_path: str):
        try:
            with open(file_path, "r") as file:
                data = json.load(file)
                circuits = list(data.keys())
                return circuits
        except FileNotFoundError:
            return "File not found"
        except json.JSONDecodeError:
            return "Error decoding JSON"
        except Exception as e:
            return str(e)
    
    def user_input(self):
        logger.info(f"Available circuits: {self.show_available_circuits(file_path)}")

        # get the circuit name from the user
        circuit_name = input("Enter the circuit name: ")

        # Usage
        circuit_info = self.get_circuit_info_from_file(file_path, circuit_name)
        return circuit_info
