#include "parameters.h"

int main (int argc, char* argv[]){
    

    

    int check_open = select_circuit();
    if (check_open == 1){
        return 1;
    }
 
    std::cout<< "\nTotal laps: "<< n_lap_tot << "\nNumber of stops: " << n_stop << "\nAverage pit stop time: " << t_stop[0] << "\nAverage time loss per kg of fuel: " << t_fuel << " s" << std::endl;

    if (n_stop==2){
        int ok = calc_strat_2stop();
    }else if (n_stop==1){
        int ok = calc_strat_1stop();
    }
    
}