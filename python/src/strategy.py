"""Strategy module for the F1 Strategy Simulator."""
from loguru import logger
from utils import degradation_calculator

class Strategy:
    def __init__(self, circuit_name: str):
        self.circuit_name = circuit_name

    def set_params(self, params):
        # Simulated parameter loading, which could be adapted to load from a file as in C
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
            params["tyre_opt_lap_hard"]
        ]
        self.tyre_degrad_coeff = [
            params["tyre_degrad_coeff_soft"],
            params["tyre_degrad_coeff_medium"],
            params["tyre_degrad_coeff_hard"]
        ]
        self.stint_lap = [0] * (self.n_stop + 1)

    def calculate_stint_times(self):
        stint_time = [[0 for _ in range(3)] for _ in range(self.n_stop + 1)]
        for stint in range(self.n_stop + 1):
            for tyre_type in range(3):
                laps = self.stint_lap[stint]
                tyre_opt = self.tyre_optimal_lap[tyre_type]
                tyre_deg = self.tyre_degrad_coeff[tyre_type]
                tyre_time = self.tyre_lap_best_time[tyre_type]
                excess_laps = max(0, laps - tyre_opt)
                t_tyre_degrad = degradation_calculator(excess_laps, tyre_deg)
                stint_time[stint][tyre_type] = laps * tyre_time + t_tyre_degrad
        return stint_time

    def adjust_stints(self, iter_num):
        increment = iter_num // 100  # Example adjustment logic
        if self.n_stop == 2:
            self.stint_lap[0] = 5 + increment
            self.stint_lap[1] = 5 + (iter_num % (self.n_lap_tot - (self.stint_lap[0] + 5)))
            self.stint_lap[2] = self.n_lap_tot - self.stint_lap[0] - self.stint_lap[1]

    def execute_strategy(self):
        best_time = float('inf')
        best_configuration = None
        for iter_num in range(self.n_iter):
            self.adjust_stints(iter_num)
            stint_times = self.calculate_stint_times()
            for config in range(27):  # 3^3 combinations
                total_time = sum(stint_times[stint][config // 3**stint % 3] for stint in range(self.n_stop + 1))
                total_time += self.stop_0_time * self.n_stop + self.fuel_time * ((self.n_lap_tot * self.fuel_con) / 2 + 1)
                if total_time < best_time:
                    best_time = total_time
                    best_configuration = [(config // 3**stint % 3) for stint in range(self.n_stop + 1)]
            logger.info(f"Iteration {iter_num+1}: Best Time {best_time} with Configuration {best_configuration}")
        return best_time, best_configuration
