from circuit_info import CircuitInfo
import strategy


from loguru import logger

CIRCUIT_FILE_PATH = '../circuit.json'


def main():

    circuit_info = CircuitInfo(
        file_path=CIRCUIT_FILE_PATH, 
        logger=logger)
    param = circuit_info.user_input()

    logger.info(f"Selected circuit: {param['circuit_name']}")
    logger.info(f"Number of laps: {param['n_lap_tot']}")
    logger.info(f"Number of iterations: {param['n_iter']}")
    logger.info(f"Number of pit stops: {param['n_stop']}")
    logger.info(f"Number of stops: {param['n_stop']}")
    logger.info(f"Average pit stop time: {param['t_stop_0']}")
    logger.info(f"Average time loss per kg of fuel: {param['t_fuel']}s")


    st = strategy.Strategy(param["circuit_name"])
    st.set_params(param)

    if param["n_stop"] == 1:
        st.one_stop_strategy(param)
    elif param["n_stop"] == 2:
        st.two_stop_strategy()


if __name__ == "__main__":
    main()
