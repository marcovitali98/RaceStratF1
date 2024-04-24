"""Main module to execute the racing strategy based on the circuit data."""

from circuit_info import CircuitInfo
from strategy import Strategy
from loguru import logger

CIRCUIT_FILE_PATH = "../circuit.json"


def load_circuit_data(file_path):
    """Load and return circuit data from the specified file path."""
    try:
        circuit_info = CircuitInfo(file_path=file_path, logger=logger)
        return circuit_info.user_input()
    except Exception as e:
        logger.error(f"Failed to load circuit data: {e}")
        raise


def log_circuit_details(param):
    """Log details of the circuit."""
    logger.info(f"Selected circuit: {param['circuit_name']}")
    logger.info(f"Number of laps: {param['n_lap_tot']}")
    logger.info(f"Number of iterations: {param['n_iter']}")
    logger.info(f"Number of pit stops: {param['n_stop']}")
    logger.info(f"Average pit stop time: {param['t_stop_0']}")
    logger.info(f"Average time loss per kg of fuel: {param['t_fuel']}s")


def execute_strategy(param):
    """Execute the racing strategy based on the number of stops."""
    strategy = Strategy(param["circuit_name"])
    strategy.set_params(param)
    if param["n_stop"] == 1:
        strategy.one_stop_strategy(param)
    elif param["n_stop"] == 2:
        strategy.two_stop_strategy()
    else:
        logger.warning(f"No strategy available for {param['n_stop']} stops.")


def main():
    param = load_circuit_data(CIRCUIT_FILE_PATH)
    if param:
        log_circuit_details(param)
        execute_strategy(param)


if __name__ == "__main__":
    main()
