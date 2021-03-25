#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constant Variable
const int guarantee_5stars_char = 90;
const int guarantee_4stars_char = 10;
const int base_5stars_char = 60; // 0.600% --> 60 / 10000
const int base_4stars_char = 510; // 5.100% --> 510 / 10000
const int range = 10000;
const int total_trail = 1000000;

// Function Prototype
int gacha();
int randRange(int n);

// Global Variable
int count_roll_from_last_4stars = 0;
int count_roll_from_last_5stars = 0;
int count_roll_until_5stars = 0;
int total_roll = 0;

int count_print = 0;

int main(int argc, char *argv[]) {
    srand(time(0));

    int sum_roll = 0;
    int count_5stars_notPity = 0;
    int count_5stars = 0;
    for (int trail = 0; trail < total_trail; trail++){
        // Gacha Time !
        int result;
        do{
            result = gacha();
        }while(result != 5 && result != 51);
        
        count_5stars++;

        if(result == 5){
            count_5stars_notPity++;
        }
        
        sum_roll += count_roll_until_5stars;
        count_roll_until_5stars = 0;
    }
    printf("Average Roll before obtaining 5 stars: %f\n", (double)sum_roll/(double)total_trail);
    printf("Chance of obtaining 5 stars (incl. pity) per roll: %d / %d = %f %%\n", count_5stars, total_roll, (double)count_5stars*100/(double)total_roll);
    printf("Chance of obtaining 5 stars (not pity) per roll: %d / %d = %f %%\n", count_5stars_notPity, total_roll, (double)count_5stars_notPity*100/(double)total_roll);
}

int gacha(){
    total_roll++;
    count_roll_until_5stars++;
    count_roll_from_last_4stars++;
    count_roll_from_last_5stars++;

    int rng = randRange(range);
    int result;
    if (count_roll_from_last_5stars == guarantee_5stars_char)
        result = 51;
    else if ((count_roll_from_last_4stars == guarantee_4stars_char) ||
             ((count_roll_from_last_5stars + 1 == guarantee_5stars_char) && (count_roll_from_last_4stars + 1 == guarantee_4stars_char)))
        result = 41;
    else if((rng >= 0) && (rng < base_5stars_char))
        result = 5;
    else if((rng >= base_5stars_char + 1) && (rng < base_5stars_char + base_4stars_char))
        result = 4;
    else
        result = 3;
    

    if (result == 5 || result == 51)
        count_roll_from_last_5stars = 0;
    else if (result == 4 || result == 41)
        count_roll_from_last_4stars = 0;
    
    count_print++;
    // printf("%3d", result);
    if((result == 5) || (result == 51)){
        // printf(" ---[%d]\n\n", count_print);
        count_print = 0;
    }
    else if(count_print % 10 == 0){
            // printf("\n");
    }
    return result;
}

int randRange(int n)
{
    int result;
    do {
        result = rand();
    }while(result >= RAND_MAX - (RAND_MAX % n));  // RAND_MAX = 32767
    return result % n;
}