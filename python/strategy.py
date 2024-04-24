
ARRAY_SIZE = 2000

TIRES = ["soft", "medium", "hard"]


class Strategy:
    def __init__(self, circuit_name: str):
        self.circuit_name = circuit_name

    def set_params(self, params):
        self.n_lap_tot = params["n_lap_tot"]
        self.n_stop = params["n_stop"]
        self.n_iter = params["n_iter"]
        self.t_fuel = params["t_fuel"]
        self.fuel_con = params["fuel_con"]
        self.t_stop_0 = params["t_stop_0"]
        self.lap_best_soft = params["lap_best_soft"]
        self.lap_best_medium = params["lap_best_medium"]
        self.lap_best_hard = params["lap_best_hard"]
        self.tyre_opt_lap_soft = params["tyre_opt_lap_soft"]
        self.tyre_opt_lap_medium = params["tyre_opt_lap_medium"]
        self.tyre_opt_lap_hard = params["tyre_opt_lap_hard"]
        self.tyre_degrad_coeff_soft = params["tyre_degrad_coeff_soft"]
        self.tyre_degrad_coeff_medium = params["tyre_degrad_coeff_medium"]
        self.tyre_degrad_coeff_hard = params["tyre_degrad_coeff_hard"]

    
    def degradation_calculator(self,laps: int, degrad_coef: float):
        result = 0
        for i in range(1, laps + 1):
            result += i * degrad_coef
        return result


    def two_stop_strategy(self):
        t_weight_fuel = self.t_fuel * ((self.n_lap_tot * self.fuel_con) / 2 + 1)
    
        for iter in range(self.n_iter):
            t_stint = [[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]]
            t_tot = [[[[0.0 for _ in range(3)] for _ in range(3)] for _ in range(3)] for _ in range(ARRAY_SIZE)]
            t_best = [0.0] * ARRAY_SIZE
            best_combo = [0] * ARRAY_SIZE
            stint_len = [0.0, 0.0, 0.0]

            if iter == 0:
                if self.n_stop == 0:
                    stint_len[0] = self.n_lap_tot
                elif self.n_stop == 1:
                    stint_len[0] = 5
                    stint_len[1] = self.n_lap_tot - 5
                elif self.n_stop == 2:
                    stint_len[0] = 5
                    stint_len[1] = 5
                    stint_len[2] = self.n_lap_tot - 10

            for stint in range(self.n_stop + 1):
                for tyre in range(3):
                    if stint_len[stint] > param[f"tyre_opt_lap_{TIRES[tyre]}"]:
                        t_tyre_degrad = self.degradation_calculator(stint_len[stint] - param[f"tyre_opt_lap_{TIRES[tyre]}"],
                                                    param[f"tyre_degrad_coeff_{TIRES[tyre]}"])
                    else:
                        t_tyre_degrad = 0
                    t_stint[stint][tyre] = stint_len[stint] * param[f"lap_best_{TIRES[tyre]}"] + t_tyre_degrad

            for i in range(pow(3, self.n_stop + 1)):
                # Calculate total time for each tire combination
                for j in range(self.n_stop):
                    t_tot[iter][(i // 9)][(i // 3) % 3][i % 3] += self.t_stop_0
                t_tot[iter][(i // 9)][(i // 3) % 3][i % 3] += t_weight_fuel

            for i in range(pow(3, self.n_stop + 1)):
                if i == 0:
                    t_best[iter] = t_tot[iter][0][0][0]
                    best_combo[iter] = 0
                else:
                    if t_tot[iter][(i // 9)][(i // 3) % 3][i % 3] < t_best[iter]:
                        t_best[iter] = t_tot[iter][(i // 9)][(i // 3) % 3][i % 3]
                        best_combo[iter] = i






            # Write to file
            """
            filePtr.write(f"\n---------------ITER {iter}---------------\n")
            for i in range(self.n_stop + 1):
                filePtr.write(f"\nSTINT {i + 1}\n")
                for j in range(3):
                    filePtr.write(f"{t_stint[i][j]} s | ")
                if i != self.n_stop:
                    filePtr.write(f"\nSTOP {i + 1}: {self.t_stop_0} s\n")

            # Print best combination
            filePtr.write(f"\nBEST COMBINATION: {best_combo[iter]}")
             """
