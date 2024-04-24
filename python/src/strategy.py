"""Strategy module for the F1 Strategy Simulator."""

from loguru import logger


ARRAY_SIZE = 2000


class Strategy:
    def __init__(self, circuit_name: str):
        self.circuit_name = circuit_name

    def set_params(self, params):
        self.n_lap_tot = params["n_lap_tot"]
        self.n_stop = params["n_stop"]
        self.n_iter = params["n_iter"]
        self.fuel_time = params["t_fuel"]
        self.fuel_con = params["fuel_con"]
        self.stop_0_time = params["t_stop_0"]
        self.tyre_lap_best_time = [
            params["lap_best_soft"],
            params["lap_best_medium"],
            params["lap_best_hard"]
        ]
        self.tyre_optimal_lap = [
            params["tyre_opt_lap_soft"],
            params["tyre_opt_lap_medium"],
            params["tyre_opt_lap_hard"],
        ]
        self.tyre_degrad_coeff = [
            params["tyre_degrad_coeff_soft"],
            params["tyre_degrad_coeff_medium"],
            params["tyre_degrad_coeff_hard"]
        ]

    def degradation_calculator(self, laps: int, degrad_coef: float) -> float:
        result = 0
        for i in range(1, laps + 1):
            result += i * degrad_coef
        return result

    def two_stop_strategy(self):
        # initialize variables
        time_weight_fuel = self.fuel_time * ((self.n_lap_tot * self.fuel_con) / 2 + 1)    
        stint_time = [[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]]
        total_time = [[[[0.0 for _ in range(3)] for _ in range(3)] for _ in range(3)] for _ in range(ARRAY_SIZE)]
        best_time = [0.0] * ARRAY_SIZE
        best_combo = [0] * ARRAY_SIZE
        stint_lap = [0.0, 0.0, 0.0]

        if self.n_stop == 0:
            stint_lap[0] = self.n_lap_tot
        elif self.n_stop == 1:
            stint_lap[0] = 5
            stint_lap[1] = self.n_lap_tot - 5
        elif self.n_stop == 2:
            stint_lap[0] = 5
            stint_lap[1] = 5
            stint_lap[2] = self.n_lap_tot - 10

        for iter in range(self.n_iter):

            # Calculate stint time
            for stint in range(self.n_stop + 1):
                for tyre_type in range(3):
                    tyre_opt = self.tyre_optimal_lap[tyre_type]
                    tyre_deg = self.tyre_degrad_coeff[tyre_type]
                    tyre_time = self.tyre_lap_best_time[tyre_type]
                    if stint_lap[stint] > tyre_opt:
                        t_tyre_degrad = self.degradation_calculator(
                            laps=stint_lap[stint] - tyre_opt, 
                            degrad_coef=tyre_deg)
                    else:
                        t_tyre_degrad = 0
                    stint_time[stint][tyre_type] = stint_lap[stint] * tyre_time + t_tyre_degrad

            # Calculate total time ì
            for i in range(pow(3, self.n_stop + 1)):
                total_time[iter][(i // 9)][(i // 3) % 3][i %3] = stint_time[0][i // 9] + stint_time[1][(i//3)%3] + stint_time[2][i%3]
                for _ in range(1, 2):
                    total_time[iter][(i // 9)][(i // 3) % 3][i % 3] += self.stop_0_time
                total_time[iter][(i // 9)][(i // 3) % 3][i % 3] += time_weight_fuel

            # Find best combination
            for i in range(pow(3, self.n_stop + 1)):
                if i == 0:
                    best_time[iter] = total_time[iter][0][0][0]
                    best_combo[iter] = 0
                else:
                    if total_time[iter][(i // 9)][(i // 3) % 3][i % 3] < best_time[iter]:
                        best_time[iter] = total_time[iter][(i // 9)][(i // 3) % 3][i % 3]
                        best_combo[iter] = i

    # to check for in all iterations if the best combination is the best in all iterations so far
    # to found code in 238-281 on C code

        def logger_output():
            # Write to file
            logger.info(f"---------------ITER {iter}---------------")
            for i in range(self.n_stop + 1):
                logger.info(f"STINT {i + 1} : {stint_lap[i]} laps")
                for j in range(3):
                    logger.info(f"{stint_time[i][j]} s | ")
                if i != self.n_stop:
                    logger.info(f"STOP {i + 1}: {self.stop_0_time} s")

            # Print best combination
            logger.info(f"BEST COMBINATION: {best_combo[iter]}")