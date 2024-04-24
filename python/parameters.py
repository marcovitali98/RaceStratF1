import os

SIZE_ARRAY = 2000

t_lap_best = [0.0, 0.0, 0.0]
t_fuel = 0.0
fuel_con = 0.0
tyre_degrad_coeff = [0.0, 0.0, 0.0]
tyre_opt_lap = [0, 0, 0]
t_stop = [0.0, 0.0]
n_lap_tot = 0.0
n_stop = 0.0
t_total_best = 0.0
combo_total_best = 0
best_iter = 0
best_stint_len = [0.0, 0.0, 0.0]
t_total_best_planb = 0.0
combo_total_best_planb = 0
best_iter_planb = 0
best_stint_len_planb = [0.0, 0.0, 0.0]
t_total_best_planc = 0.0
combo_total_best_planc = 0
best_iter_planc = 0
best_stint_len_planc = [0.0, 0.0, 0.0]
n_iter = 0

TIRES = ["soft", "medium", "hard"]


def power(base, exponent):
    return base ** exponent


def degrad_calc(laps, degrad_coef):
    result = 0
    for i in range(1, laps + 1):
        result += i * degrad_coef
    return result


def conv_time(seconds):
    hh = int(seconds) // 3600
    mm = (int(seconds) % 3600) // 60
    ss = int(seconds) % 60
    mill = int(seconds * 1000) % 1000
    return f"{hh:02d}:{mm:02d}:{ss:02d}.{mill:03d}s"


def extract_parameters_from_file(file_path):
    parameters = {}

    try:
        with open(file_path, 'r') as file:
            for line in file:
                key, value = line.strip().split()
                parameters[key] = float(value) if '.' in value else int(value)
    except FileNotFoundError:
        print("File not found:", file_path)

    return parameters

def select_circuit():
    name = input("\nSelect circuit: ")
    current_dir = os.getcwd()

    file_path = os.path.join("./circuits", name+".txt")  # Replace with the path to your file
    parameters = extract_parameters_from_file(file_path)

    return parameters



def calc_strat_2stop(param):
    t_weight_fuel = param["t_fuel"] * ((param["n_lap_tot"] * param["fuel_con"]) / 2 + 1)
    with open("data.txt", "w") as filePtr:
        for iter in range(param["n_iter"]):
            t_stint = [[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]]
            t_tot = [[[[0.0 for _ in range(3)] for _ in range(3)] for _ in range(3)] for _ in range(SIZE_ARRAY)]
            t_best = [0.0] * SIZE_ARRAY
            best_combo = [0] * SIZE_ARRAY
            stint_len = [0.0, 0.0, 0.0]

            if iter == 0:
                if param["n_stop"] == 0:
                    stint_len[0] = param["n_lap_tot"]
                elif param["n_stop"] == 1:
                    stint_len[0] = 5
                    stint_len[1] = param["n_lap_tot"] - 5
                elif param["n_stop"] == 2:
                    stint_len[0] = 5
                    stint_len[1] = 5
                    stint_len[2] = param["n_lap_tot"] - 10

            for stint in range(param["n_stop"] + 1):
                for tyre in range(3):
                    if stint_len[stint] > param[f"tyre_opt_lap_{TIRES[tyre]}"]:
                        t_tyre_degrad = degrad_calc(stint_len[stint] - param[f"tyre_opt_lap_{TIRES[tyre]}"],
                                                    param[f"tyre_degrad_coeff_{TIRES[tyre]}"])
                    else:
                        t_tyre_degrad = 0
                    t_stint[stint][tyre] = stint_len[stint] * param[f"lap_best_{TIRES[tyre]}"] + t_tyre_degrad

            for i in range(pow(3, param["n_stop"] + 1)):
                # Calculate total time for each tire combination
                for j in range(param["n_stop"]):
                    t_tot[iter][(i // 9)][(i // 3) % 3][i % 3] += param["t_stop_0"]
                t_tot[iter][(i // 9)][(i // 3) % 3][i % 3] += t_weight_fuel

            for i in range(pow(3, param["n_stop"] + 1)):
                if i == 0:
                    t_best[iter] = t_tot[iter][0][0][0]
                    best_combo[iter] = 0
                else:
                    if t_tot[iter][(i // 9)][(i // 3) % 3][i % 3] < t_best[iter]:
                        t_best[iter] = t_tot[iter][(i // 9)][(i // 3) % 3][i % 3]
                        best_combo[iter] = i

            # Write to file
            filePtr.write(f"\n---------------ITER {iter}---------------\n")
            for i in range(param["n_stop"] + 1):
                filePtr.write(f"\nSTINT {i + 1}\n")
                for j in range(3):
                    filePtr.write(f"{t_stint[i][j]} s | ")
                if i != param["n_stop"]:
                    filePtr.write(f"\nSTOP {i + 1}: {param['t_stop_0']} s\n")

            # Print best combination
            filePtr.write(f"\nBEST COMBINATION: {best_combo[iter]}")
