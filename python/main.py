import parameters

def main():
    param = parameters.select_circuit()

    print("\nTotal laps:", param["n_lap_tot"])
    print("Number of stops:", param["n_stop"])
    print("Average pit stop time:", param["t_stop_0"])
    print("Average time loss per kg of fuel:", param["t_fuel"], "s")

    if param["n_stop"] == 2:
        ok = parameters.calc_strat_2stop(param)
    elif param["n_stop"] == 1:
        ok = parameters.calc_strat_1stop()

if __name__ == "__main__":
    main()
