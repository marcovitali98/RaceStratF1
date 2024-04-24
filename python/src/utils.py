def degradation_calculator(laps: int, degrad_coef: float) -> float:
    result = 0
    for i in range(1, laps + 1):
        result += i * degrad_coef
    return result