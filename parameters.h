#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <filesystem>

#define SIZE_ARRAY 2000

/*float   t_lap_best[3]           = {100, 100.5, 101};            // tempo ideale giro con ogni gomma
float   t_fuel                  = 0.03;                         // perdita tempo sul giro per ogni kg di carburante
float   fuel_con                = 3;                            // consumo di kg di carburante per giro
float   tyre_degrad_coeff[3]    = {0.5, 0.25, 0.1};             // 0 soft, 1 medium, 2 hard
int     tyre_opt_lap[3]         = {9, 16, 30};                  // 0 soft, 1 medium, 2 hard
float   t_stop[2]               = {25, 35};
float   n_lap_tot               = 50;
float   n_stop                  = 2;
float   t_total_best;
int     combo_total_best;
int     best_iter;
float   best_stint_len[3];
int     n_iter                  = 1600;                           // iterazioni del metodo*/

float   t_lap_best[3];            // tempo ideale giro con ogni gomma
float   t_fuel                 ;                         // perdita tempo sul giro per ogni kg di carburante
float   fuel_con              ;                            // consumo di kg di carburante per giro
float   tyre_degrad_coeff[3]  ;             // 0 soft, 1 medium, 2 hard
int     tyre_opt_lap[3]       ;                  // 0 soft, 1 medium, 2 hard
float   t_stop[2]           ;
float   n_lap_tot          ;
float   n_stop             ;
float   t_total_best;
int     combo_total_best;
int     best_iter;
float   best_stint_len[3];
float   t_total_best_planb;
int     combo_total_best_planb;
int     best_iter_planb;
float   best_stint_len_planb[3];
float   t_total_best_planc;
int     combo_total_best_planc;
int     best_iter_planc;
float   best_stint_len_planc[3];
int     n_iter            ;                           // iterazioni del metodo

std::string tires[] = {"SOFT", "MEDIUM", "HARD"};

int power(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; ++i) {
        result *= base;
    }
    return result;
}

float degrad_calc(int laps, float degrad_coef) {
    float result = 0;
    for (int i = 1; i <= laps; ++i) {
        result = result + i*degrad_coef;            // esempio: dopo il numero ottimale di giri, il primo giro extra perdo 0.1, il secondo 0.2, ecc.
    }
    return result;
}

std::string conv_time(float seconds){
    std::ostringstream time;
    int hh, mm, ss, mill;
    hh = int(seconds)/3600;
    mm = (int(seconds)%3600)/60;
    ss = int(seconds)%60;
    mill = int(seconds*1000)%1000;
    time    << std::setw(2) << std::setfill('0') << hh << ":"
            << std::setw(2) << std::setfill('0') << mm << ":"
            << std::setw(2) << std::setfill('0') << ss << "."
            << std::setw(3) << std::setfill('0') << mill << "s";
    return time.str();
}

int select_circuit(){
    std::string name;

    std::cout << "\nCurrently supported circuits:\nCatalunya\n";
    std::cout << "\nSelect circuit: ";
    std::cin >> name;

    std::filesystem::path currentDir = std::filesystem::current_path();

    // Compose the path to the file
    std::string filePath = (currentDir / "circuits" / (name + ".txt")).string();

    std::ifstream circuit_file_ptr(filePath); // Open the file for reading
    if (!circuit_file_ptr.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    std::string paramName;
    while (circuit_file_ptr >> paramName) { // Read parameter names
        if (paramName == "lap_best_soft") {
            circuit_file_ptr >> t_lap_best[0];
        } else if (paramName == "lap_best_medium") {
            circuit_file_ptr >> t_lap_best[1];
        } else if (paramName == "lap_best_hard") {
            circuit_file_ptr >> t_lap_best[2];
        } else if (paramName == "t_fuel") {
            circuit_file_ptr >> t_fuel;
        } else if (paramName == "fuel_con") {
            circuit_file_ptr >> fuel_con;
        } else if (paramName == "tyre_degrad_coeff_soft") {
            circuit_file_ptr >> tyre_degrad_coeff[0];
        } else if (paramName == "tyre_degrad_coeff_medium") {
            circuit_file_ptr >> tyre_degrad_coeff[1];
        } else if (paramName == "tyre_degrad_coeff_hard") {
            circuit_file_ptr >> tyre_degrad_coeff[2];
        } else if (paramName == "tyre_opt_lap_soft") {
            circuit_file_ptr >> tyre_opt_lap[0];
        } else if (paramName == "tyre_opt_lap_medium") {
            circuit_file_ptr >> tyre_opt_lap[1];
        } else if (paramName == "tyre_opt_lap_hard") {
            circuit_file_ptr >> tyre_opt_lap[2];
        } else if (paramName == "t_stop_0") {
            circuit_file_ptr >> t_stop[0];
        } else if (paramName == "n_lap_tot") {
            circuit_file_ptr >> n_lap_tot;
        } else if (paramName == "n_stop") {
            circuit_file_ptr >> n_stop;
        } else if (paramName == "n_iter") {
            circuit_file_ptr >> n_iter;
        }
    }
    return 0;                        
}

int calc_strat_2stop(){
    float t_weight_fuel;
    t_weight_fuel = t_fuel*((n_lap_tot*fuel_con)/2+1);

    FILE *filePtr;
    filePtr = fopen("data.txt", "w");
    if (filePtr == nullptr) {
        printf("Error opening file!\n");
        return 1;
    }

    // Iterazioni per Plan A e Plan B (2 stop)
    for (int iter=0; iter<n_iter; iter++){
        float t_stint[3][3] = {{0,0,0},{0,0,0},{0,0,0}};    // max 3 stint, per ognuno calcolo il tempo con ogni tipo di gomma
        float t_tot[SIZE_ARRAY][3][3][3];
        float t_best[SIZE_ARRAY];
        int best_combo[SIZE_ARRAY];
        float stint_len[3];

        if (iter==0){
            // Guess iniziale
            if (n_stop==0){
                stint_len[0] = n_lap_tot;
            }else if(n_stop==1){
                stint_len[0] = 5;
                stint_len[1] = n_lap_tot-5;
            }else if(n_stop==2){
                stint_len[0] = 5;
                stint_len[1] = 5;
                stint_len[2] = n_lap_tot-10;
            }
        }

        fprintf(filePtr,"\n---------------ITER %d---------------\n", iter);

        // Calcolo tempi stint per singola iterazione
        for (int stint = 0; stint<=n_stop; stint++){
            float t_tyre_degrad;

            for (int tyre=0; tyre<3; tyre++){                                                                               // tempo stint coi 3 tipi di gomma
                if (stint_len[stint] > tyre_opt_lap[tyre]){
                    //t_tyre_degrad = (stint_len[stint]-tyre_opt_lap[tyre])*                  tyre_degrad_coeff[tyre];                    // degrado gomma dopo i giri ottimali della mescola
                    t_tyre_degrad = degrad_calc((stint_len[stint]-tyre_opt_lap[tyre]), tyre_degrad_coeff[tyre]);
                }else{
                    t_tyre_degrad = 0;
                }
                t_stint[stint][tyre]    = stint_len[stint]*t_lap_best[tyre] + t_tyre_degrad;
            }

            /*if (stint!=n_stop){
                t_stop[stint] = 20 + t_refuel*stint_len[stint];
            }*/

        }

        for (int i=0; i<=n_stop; i++){
            fprintf(filePtr,"\nSTINT %d\n", (i+1));
            fprintf(filePtr,"Soft: %f s | Medium: %f s | Hard: %f s\n", t_stint[i][0], t_stint[i][1], t_stint[i][2]);

            if (i!=n_stop){
                fprintf(filePtr,"\nSTOP %d: %f s\n", (i+1), t_stop[0]);
            }
        }

        for (int i=0; i<=power(3, (n_stop+1)); i++){                                    // combinazioni di gomme possibili: 3 ** n_stop
            /*for (int j=0; j<=n_stop; j++){                                          // 
                t_tot[iter][(i / 9)][i % 3][i / 9] = t_tot[iter][(i / 9)][i % 3][i / 9] + t_stint[j][i];     // S-S, S-M, S-H, M-S, M-M, M-H, H-S, H-M, H-H
                
            }*/
            if (n_stop == 0){
                t_tot[iter][(i / 9)][(i / 3)%3][i %3] = t_stint[0][(i/3)];
            }else if (n_stop == 1){
                t_tot[iter][(i / 9)][(i / 3)%3][i %3] = t_stint[0][(i/3)%3] + t_stint[1][i%3];
            }else if (n_stop == 2){
                t_tot[iter][(i / 9)][(i / 3)%3][i %3] = t_stint[0][i/9] + t_stint[1][(i/3)%3] + t_stint[2][i%3];
            }

            for (int j=0; j<n_stop; j++){
                t_tot[iter][(i/9)][(i/3)%3][i%3] = t_tot[iter][(i/9)][(i/3)%3][i %3] + t_stop[0];
            }
            t_tot[iter][(i/9)][(i/3)%3][i %3] = t_tot[iter][(i / 9)][(i / 3)%3][i %3] + t_weight_fuel;

        }

        for (int i=0; i<power(3, (n_stop+1)); i++){
            if (i==0){
                t_best[iter]    = t_tot[iter][0][0][0];
                best_combo[iter]      = 0;
            }else{
                if (t_tot[iter][(i/9)][(i/3)%3][i %3] < t_best[iter]){
                    t_best[iter] = t_tot[iter][(i/9)][(i/3)%3][i %3];
                    best_combo[iter]      = i;
                }
            }
        }

 
        if (n_stop==0){
            fprintf(filePtr,"\nBEST COMBINATION: %s (%d), TIME: %f s", tires[(best_combo[iter]/3)].c_str(), int(stint_len[0]), t_best[iter]);
        }else if(n_stop==1){
            fprintf(filePtr,"\nBEST COMBINATION: %s (%d) - %s (%d), TIME: %f s", tires[(best_combo[iter]/3)].c_str(), int(stint_len[0]), tires[(best_combo[iter]%3)].c_str(), int(stint_len[1]), t_best[iter]);
        }else if(n_stop==2){
            fprintf(filePtr,"\nBEST COMBINATION: %s (%d) - %s (%d) - %s (%d), TIME: %f s\n", tires[(best_combo[iter]/9)].c_str(), int(stint_len[0]), tires[(best_combo[iter]/3)%3].c_str(), int(stint_len[1]), tires[(best_combo[iter]%3)].c_str(), int(stint_len[2]), t_best[iter]);
        }

        if (iter==0){
            t_total_best = t_best[0];
            combo_total_best = best_combo[0];
            best_iter = 0;
            best_stint_len[0] = stint_len[0];
            best_stint_len[1] = stint_len[1];
            best_stint_len[2] = stint_len[2];
        }else if(t_best[iter]<t_total_best){
            if      ((
                        (((combo_total_best/9)==2)||((combo_total_best/3)%3==2)||((combo_total_best%3)==2)) && (((best_combo[iter]/9)==1)||((best_combo[iter]/3)%3==1)||((best_combo[iter]%3)==1))
                    ) || (
                        (((combo_total_best/9)==1)||((combo_total_best/3)%3==1)||((combo_total_best%3)==1)) && (((best_combo[iter]/9)==2)||((best_combo[iter]/3)%3==2)||((best_combo[iter]%3)==2))
                    )
                    ){
                t_total_best_planb = t_total_best;
                combo_total_best_planb = combo_total_best;
                best_iter_planb = best_iter;
                best_stint_len_planb[0] = best_stint_len[0];
                best_stint_len_planb[1] = best_stint_len[1];
                best_stint_len_planb[2] = best_stint_len[2];
            }

            t_total_best = t_best[iter];
            combo_total_best = best_combo[iter];
            best_iter = iter;
            best_stint_len[0] = stint_len[0];
            best_stint_len[1] = stint_len[1];
            best_stint_len[2] = stint_len[2];
            
        }else if (t_best[iter]<t_total_best_planb){
            if      ((
                        (((combo_total_best/9)==2)||((combo_total_best/3)%3==2)||((combo_total_best%3)==2)) && (((best_combo[iter]/9)==1)||((best_combo[iter]/3)%3==1)||((best_combo[iter]%3)==1))
                    ) || (
                        (((combo_total_best/9)==1)||((combo_total_best/3)%3==1)||((combo_total_best%3)==1)) && (((best_combo[iter]/9)==2)||((best_combo[iter]/3)%3==2)||((best_combo[iter]%3)==2))
                    )
                    ){
                t_total_best_planb = t_best[iter];
                combo_total_best_planb = best_combo[iter];
                best_iter_planb = iter;
                best_stint_len_planb[0] = stint_len[0];
                best_stint_len_planb[1] = stint_len[1];
                best_stint_len_planb[2] = stint_len[2];
            }
        }

        if (n_stop==0 && stint_len[0]<(n_lap_tot-5)){
            stint_len[0]++;
        }else if(n_stop==1 && stint_len[0]<(n_lap_tot-5) && stint_len[1]>5){
            stint_len[0]++;
            stint_len[1]--;
        }else if(n_stop==2){

            if (stint_len[0]<(n_lap_tot-15)){
                stint_len[0] = 5 + iter / (int(n_lap_tot)-15);

                //if (stint_len[1]<(n_lap_tot-15)){
                    stint_len[1] = 5 + iter % (int(n_lap_tot-(stint_len[0] + 5)));

                    //if (stint_len[2]>5){
                        stint_len[2] = n_lap_tot - stint_len[0] - stint_len[1];
                    //}else{
                    //    break;
                    //}
                //}else{
                    
                //}
            }else{
                break;
            }
            
        }

    }

    // Iterazioni per Plan C (1 stop)
    for (int iter=0; iter<n_iter; iter++){
        float t_stint[3][3] = {{0,0,0},{0,0,0},{0,0,0}};    // max 3 stint, per ognuno calcolo il tempo con ogni tipo di gomma
        float t_tot[SIZE_ARRAY][3][3][3];
        float t_best[SIZE_ARRAY];
        int best_combo[SIZE_ARRAY];
        float stint_len[3];

        if (iter==0){
            // Guess iniziale
            stint_len[0] = 5;
            stint_len[1] = n_lap_tot-5;
        }

        fprintf(filePtr,"\n---------------PLAN C----------------\n");
        fprintf(filePtr,"\n---------------ITER %d---------------\n", iter);

        // Calcolo tempi stint per singola iterazione
        for (int stint = 0; stint<=1; stint++){
            float t_tyre_degrad;

            for (int tyre=0; tyre<3; tyre++){                                                                               // tempo stint coi 3 tipi di gomma
                if (stint_len[stint] > tyre_opt_lap[tyre]){
                    //t_tyre_degrad = (stint_len[stint]-tyre_opt_lap[tyre])*                  tyre_degrad_coeff[tyre];                    // degrado gomma dopo i giri ottimali della mescola
                    t_tyre_degrad = degrad_calc((stint_len[stint]-tyre_opt_lap[tyre]), tyre_degrad_coeff[tyre]);
                }else{
                    t_tyre_degrad = 0;
                }
                t_stint[stint][tyre]    = stint_len[stint]*t_lap_best[tyre] + t_tyre_degrad;
            }

        }

        for (int i=0; i<=1; i++){
            fprintf(filePtr,"\nSTINT %d\n", (i+1));
            fprintf(filePtr,"Soft: %f s | Medium: %f s | Hard: %f s\n", t_stint[i][0], t_stint[i][1], t_stint[i][2]);

            if (i!=1){
                fprintf(filePtr,"\nSTOP %d: %f s\n", (i+1), t_stop[0]);
            }
        }

        for (int i=0; i<=power(3, (1+1)); i++){                                    // combinazioni di gomme possibili: 3

            t_tot[iter][(i/9)][(i/3)%3][i%3] = t_stint[0][(i/3)%3] + t_stint[1][i%3];

            t_tot[iter][(i/9)][(i/3)%3][i%3] = t_tot[iter][(i/9)][(i/3)%3][i%3] + t_stop[0];
            t_tot[iter][(i/9)][(i/3)%3][i%3] = t_tot[iter][(i/9)][(i/3)%3][i%3] + t_weight_fuel;

        }

        for (int i=0; i<power(3, (1+1)); i++){
            if (i==0){
                t_best[iter]            = t_tot[iter][0][0][0];
                best_combo[iter]        = 0;
            }else{
                if (t_tot[iter][(i/9)][(i/3)%3][i %3] < t_best[iter]){
                    t_best[iter] = t_tot[iter][(i/9)][(i/3)%3][i%3];
                    best_combo[iter]    = i;
                }
            }
        }

 
        fprintf(filePtr,"\nBEST COMBINATION: %s (%d) - %s (%d), TIME: %f s", tires[(best_combo[iter]/3)].c_str(), int(stint_len[0]), tires[(best_combo[iter]%3)].c_str(), int(stint_len[1]), t_best[iter]);

        if (iter==0){
            t_total_best_planc = t_best[0];
            combo_total_best_planc = best_combo[0];
            best_iter_planc = 0;
            best_stint_len_planc[0] = stint_len[0];
            best_stint_len_planc[1] = stint_len[1];
            best_stint_len_planc[2] = stint_len[2];
        }else if(t_best[iter]<t_total_best_planc){
            if ((((combo_total_best_planc/3)==2)||((combo_total_best_planc%3)==2)) && (((best_combo[iter]/3)==1)||((best_combo[iter]%3)==1)) && ((((combo_total_best_planc/3)==0) != ((combo_total_best_planc%3)==0)))){
                t_total_best_planc = t_best[iter];
                combo_total_best_planc = best_combo[iter];
                best_iter_planc = iter;
                best_stint_len_planc[0] = stint_len[0];
                best_stint_len_planc[1] = stint_len[1];
                best_stint_len_planc[2] = stint_len[2];
            }
        }

        if(stint_len[0]<(n_lap_tot-5) && stint_len[1]>5){
            stint_len[0]++;
            stint_len[1]--;
        }

    }

    printf("\n------------------------------------\n");

    if (n_stop==0){
        printf("\nSUGGESTED PLAN A: %d\n%s (%d), TIME: %s", best_iter, tires[(combo_total_best%3)].c_str(), int(best_stint_len[0]), conv_time(t_total_best).c_str());
    }else if(n_stop==1){
        printf("\nSUGGESTED PLAN A: %d\n%s (%d) - %s (%d), TIME: %s", best_iter, tires[(combo_total_best/3)].c_str(), int(best_stint_len[0]), tires[(combo_total_best%3)].c_str(), int(best_stint_len[1]), conv_time(t_total_best).c_str());
    }else if(n_stop==2){
        printf("\nSUGGESTED PLAN A: %d\n%s (%d) - %s (%d) - %s (%d), TIME: %s", best_iter, tires[(combo_total_best/9)].c_str(), int(best_stint_len[0]), tires[(combo_total_best/3)%3].c_str(), int(best_stint_len[1]), tires[(combo_total_best%3)].c_str(), int(best_stint_len[2]), conv_time(t_total_best).c_str());
    }

    printf("\n\n------------------------------------\n");

    if (n_stop==0){
        printf("\nSUGGESTED PLAN B: %d\n%s (%d), TIME: %s", best_iter_planb, tires[(combo_total_best_planb%3)].c_str(), int(best_stint_len_planb[0]), conv_time(t_total_best_planb).c_str());
    }else if(n_stop==1){
        printf("\nSUGGESTED PLAN B: %d\n%s (%d) - %s (%d), TIME: %s", best_iter_planb, tires[(combo_total_best_planb/3)].c_str(), int(best_stint_len_planb[0]), tires[(combo_total_best_planb%3)].c_str(), int(best_stint_len_planb[1]), conv_time(t_total_best_planb).c_str());
    }else if(n_stop==2){
        printf("\nSUGGESTED PLAN B: %d\n%s (%d) - %s (%d) - %s (%d), TIME: %s", best_iter_planb, tires[(combo_total_best_planb/9)].c_str(), int(best_stint_len_planb[0]), tires[(combo_total_best_planb/3)%3].c_str(), int(best_stint_len_planb[1]), tires[(combo_total_best_planb%3)].c_str(), int(best_stint_len_planb[2]), conv_time(t_total_best_planb).c_str());
    }

    printf("\n\n------------------------------------\n");

    printf("\nSUGGESTED PLAN C: %d\n%s (%d) - %s (%d), TIME: %s", best_iter_planc, tires[(combo_total_best_planc/3)].c_str(), int(best_stint_len_planc[0]), tires[(combo_total_best_planc%3)].c_str(), int(best_stint_len_planc[1]), conv_time(t_total_best_planc).c_str());

    printf("\n\n------------------------------------\n");

    fclose(filePtr);

    return 0;
}

int calc_strat_1stop(){
    float t_weight_fuel;
    t_weight_fuel = t_fuel*((n_lap_tot*fuel_con)/2+1);

    FILE *filePtr;
    filePtr = fopen("data.txt", "w");
    if (filePtr == nullptr) {
        printf("Error opening file!\n");
        return 1;
    }

    // Iterazioni per Plan A e Plan B (1 stop)
    for (int iter=0; iter<n_iter; iter++){
        float t_stint[3][3] = {{0,0,0},{0,0,0},{0,0,0}};    // max 3 stint, per ognuno calcolo il tempo con ogni tipo di gomma
        float t_tot[SIZE_ARRAY][3][3][3];
        float t_best[SIZE_ARRAY];
        int best_combo[SIZE_ARRAY];
        float stint_len[3];

        if (iter==0){
            // Guess iniziale
            stint_len[0] = 5;
            stint_len[1] = n_lap_tot-5;
        }

        fprintf(filePtr,"\n---------------ITER %d---------------\n", iter);

        // Calcolo tempi stint per singola iterazione
        for (int stint = 0; stint<=1; stint++){
            float t_tyre_degrad;

            for (int tyre=0; tyre<3; tyre++){                                                                               // tempo stint coi 3 tipi di gomma
                if (stint_len[stint] > tyre_opt_lap[tyre]){
                    //t_tyre_degrad = (stint_len[stint]-tyre_opt_lap[tyre])*                  tyre_degrad_coeff[tyre];                    // degrado gomma dopo i giri ottimali della mescola
                    t_tyre_degrad = degrad_calc((stint_len[stint]-tyre_opt_lap[tyre]), tyre_degrad_coeff[tyre]);
                }else{
                    t_tyre_degrad = 0;
                }
                t_stint[stint][tyre]    = stint_len[stint]*t_lap_best[tyre] + t_tyre_degrad;
            }
        }

        for (int i=0; i<=1; i++){
            fprintf(filePtr,"\nSTINT %d\n", (i+1));
            fprintf(filePtr,"Soft: %f s | Medium: %f s | Hard: %f s\n", t_stint[i][0], t_stint[i][1], t_stint[i][2]);

            if (i!=1){
                fprintf(filePtr,"\nSTOP %d: %f s\n", (i+1), t_stop[0]);
            }
        }

        for (int i=0; i<=power(3, (1+1)); i++){                                    // combinazioni di gomme possibili: 3 ** n_stop
            
            t_tot[iter][(i / 9)][(i / 3)%3][i %3] = t_stint[0][(i/3)%3] + t_stint[1][i%3];
            t_tot[iter][(i/9)][(i/3)%3][i%3] = t_tot[iter][(i/9)][(i/3)%3][i%3] + t_stop[0];
            t_tot[iter][(i/9)][(i/3)%3][i%3] = t_tot[iter][(i/9)][(i/3)%3][i%3] + t_weight_fuel;

        }

        for (int i=0; i<power(3, (1+1)); i++){
            if (i==0){
                t_best[iter]    = t_tot[iter][0][0][0];
                best_combo[iter]      = 0;
            }else{
                if (t_tot[iter][(i/9)][(i/3)%3][i %3] < t_best[iter]){
                    t_best[iter] = t_tot[iter][(i/9)][(i/3)%3][i %3];
                    best_combo[iter]      = i;
                }
            }
        }

 
        fprintf(filePtr,"\nBEST COMBINATION: %s (%d) - %s (%d), TIME: %f s", tires[(best_combo[iter]/3)].c_str(), int(stint_len[0]), tires[(best_combo[iter]%3)].c_str(), int(stint_len[1]), t_best[iter]);

        if (iter==0){
            t_total_best = t_best[0];
            combo_total_best = best_combo[0];
            best_iter = 0;
            best_stint_len[0] = stint_len[0];
            best_stint_len[1] = stint_len[1];
            best_stint_len[2] = stint_len[2];
        }else if((t_best[iter]<t_total_best)  && (((best_combo[iter]/3) != (best_combo[iter]%3))) && (best_combo[iter] != combo_total_best)){
            if      ((
                        (((combo_total_best/9)==2)||((combo_total_best/3)%3==2)||((combo_total_best%3)==2)) && (((best_combo[iter]/9)==1)||((best_combo[iter]/3)%3==1)||((best_combo[iter]%3)==1))
                    ) || (
                        (((combo_total_best/9)==1)||((combo_total_best/3)%3==1)||((combo_total_best%3)==1)) && (((best_combo[iter]/9)==2)||((best_combo[iter]/3)%3==2)||((best_combo[iter]%3)==2))
                    )
                    ){
                t_total_best_planb = t_total_best;
                combo_total_best_planb = combo_total_best;
                best_iter_planb = best_iter;
                best_stint_len_planb[0] = best_stint_len[0];
                best_stint_len_planb[1] = best_stint_len[1];
                best_stint_len_planb[2] = best_stint_len[2];
            }

            t_total_best = t_best[iter];
            combo_total_best = best_combo[iter];
            best_iter = iter;
            best_stint_len[0] = stint_len[0];
            best_stint_len[1] = stint_len[1];
            best_stint_len[2] = stint_len[2];
            
        }else if ((t_best[iter]<t_total_best_planb)  && (((best_combo[iter]/3) != (best_combo[iter]%3))) && (t_best[iter]>t_total_best) && (best_combo[iter] != combo_total_best)){
            if      ((
                        (((combo_total_best/9)==2)||((combo_total_best/3)%3==2)||((combo_total_best%3)==2)) && (((best_combo[iter]/9)==1)||((best_combo[iter]/3)%3==1)||((best_combo[iter]%3)==1))
                    ) || (
                        (((combo_total_best/9)==1)||((combo_total_best/3)%3==1)||((combo_total_best%3)==1)) && (((best_combo[iter]/9)==2)||((best_combo[iter]/3)%3==2)||((best_combo[iter]%3)==2))
                    )
                    ){
                t_total_best_planb = t_best[iter];
                combo_total_best_planb = best_combo[iter];
                best_iter_planb = iter;
                best_stint_len_planb[0] = stint_len[0];
                best_stint_len_planb[1] = stint_len[1];
                best_stint_len_planb[2] = stint_len[2];
            }
        }

        if (n_stop==0 && stint_len[0]<(n_lap_tot-5)){
            stint_len[0]++;
        }else if(n_stop==1 && stint_len[0]<(n_lap_tot-5) && stint_len[1]>5){
            stint_len[0]++;
            stint_len[1]--;
        }else if(n_stop==2){

            if (stint_len[0]<(n_lap_tot-15)){
                stint_len[0] = 5 + iter / (int(n_lap_tot)-15);

                //if (stint_len[1]<(n_lap_tot-15)){
                    stint_len[1] = 5 + iter % (int(n_lap_tot-(stint_len[0] + 5)));

                    //if (stint_len[2]>5){
                        stint_len[2] = n_lap_tot - stint_len[0] - stint_len[1];
                    //}else{
                    //    break;
                    //}
                //}else{
                    
                //}
            }else{
                break;
            }
            
        }

    }

    printf("\n------------------------------------\n");

    if (n_stop==0){
        printf("\nSUGGESTED PLAN A: %d\n%s (%d), TIME: %s", best_iter, tires[(combo_total_best%3)].c_str(), int(best_stint_len[0]), conv_time(t_total_best).c_str());
    }else if(n_stop==1){
        printf("\nSUGGESTED PLAN A: %d\n%s (%d) - %s (%d), TIME: %s", best_iter, tires[(combo_total_best/3)].c_str(), int(best_stint_len[0]), tires[(combo_total_best%3)].c_str(), int(best_stint_len[1]), conv_time(t_total_best).c_str());
    }else if(n_stop==2){
        printf("\nSUGGESTED PLAN A: %d\n%s (%d) - %s (%d) - %s (%d), TIME: %s", best_iter, tires[(combo_total_best/9)].c_str(), int(best_stint_len[0]), tires[(combo_total_best/3)%3].c_str(), int(best_stint_len[1]), tires[(combo_total_best%3)].c_str(), int(best_stint_len[2]), conv_time(t_total_best).c_str());
    }

    printf("\n\n------------------------------------\n");

    if (n_stop==0){
        printf("\nSUGGESTED PLAN B: %d\n%s (%d), TIME: %s", best_iter_planb, tires[(combo_total_best_planb%3)].c_str(), int(best_stint_len_planb[0]), conv_time(t_total_best_planb).c_str());
    }else if(n_stop==1){
        printf("\nSUGGESTED PLAN B: %d\n%s (%d) - %s (%d), TIME: %s", best_iter_planb, tires[(combo_total_best_planb/3)].c_str(), int(best_stint_len_planb[0]), tires[(combo_total_best_planb%3)].c_str(), int(best_stint_len_planb[1]), conv_time(t_total_best_planb).c_str());
    }else if(n_stop==2){
        printf("\nSUGGESTED PLAN B: %d\n%s (%d) - %s (%d) - %s (%d), TIME: %s", best_iter_planb, tires[(combo_total_best_planb/9)].c_str(), int(best_stint_len_planb[0]), tires[(combo_total_best_planb/3)%3].c_str(), int(best_stint_len_planb[1]), tires[(combo_total_best_planb%3)].c_str(), int(best_stint_len_planb[2]), conv_time(t_total_best_planb).c_str());
    }

    printf("\n\n------------------------------------\n");

    fclose(filePtr);

    return 0;
}