import strategy

from params_reader import CircuitInfo


def main():

    circuit_info = CircuitInfo()
    param = circuit_info.user_input()

    print("\nTotal laps:", param["n_lap_tot"])
    print("Number of stops:", param["n_stop"])
    print("Average pit stop time:", param["t_stop_0"])
    print("Average time loss per kg of fuel:", param["t_fuel"], "s")


    st = strategy.Strategy(param["circuit_name"])
    st.set_params(param)

    if param["n_stop"] == 1:
        st.one_stop_strategy(param)
    elif param["n_stop"] == 2:
        st.two_stop_strategy()


if __name__ == "__main__":
    main()
