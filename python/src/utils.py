def degradation_calculator(laps: int, degrad_coef: float) -> float:
    """Calculate the total degradation over a number of laps based on a degradation coefficient.
    
    Args:
        laps (int): The total number of laps.
        degrad_coef (float): The degradation coefficient per lap.

    Returns:
        float: The total degradation accumulated over the laps.
    """
    sum_of_laps = laps * (laps + 1) / 2
    return sum_of_laps * degrad_coef
