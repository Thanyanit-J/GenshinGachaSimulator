#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constant Variable
const double base_rate_5star = 0.006;     // 0.6% = 0.006
const double softpity_rate_5star = 0.324;
const int guarantee_5star = 90;
const int softpity_5star = 76;

const double base_rate_4star = 0.051;    // 5.1% = 0.051
const double softpity_rate_4star = 0.511;
const int guarantee_4star = 10;
const int softpity_4star = 9;

const int total_attempt = 1000000;

const bool displayPull = false;

// Function Prototypes
int gacha();
int randomItemRarity();
void wishCountIncrement();
void updatePityStatus();
double currentRate5star();
double currentRate4star();

/** RNG in this code is Mersenne Twister pseudo random number generator by Makoto Matsumoto and Takuji Nishimura
 * This version of Mersenne Twister is based on Mersenne prime 2**19937 − 1
 * This C code is from http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/ARTICLES/mt.pdf
 * 
 * This Genshin Impact gacha simulator uses MT19937 algorithm according to the post https://bbs.mihoyo.com/ys/article/2102609
 */
void sgenrand(unsigned long seed);
double genrand();

// Global Variables
int count_roll_from_last_4star = 0;
int count_roll_from_last_5star = 0;
int total_roll = 0;
bool isGuarantee5star = false;
bool isGuarantee4star = false;
bool isSoftPity5star = false;
bool isSoftPity4star = false;

int main(int argc, char *argv[]) {
    // Using current machine time as a seed for RNG function.
    sgenrand(time(0));

    int sum_roll = 0;
    int count_5star = 0;
    int count_5star_notPity = 0;
    int count_5star_softPity = 0;
    int count_5star_guarantee = 0;
    int count_4star = 0;
    int count_4star_notPity = 0;
    int count_4star_softPity = 0;
    int count_4star_guarantee = 0;
    int count_3star = 0;

    int count_printed_item = 0;

    for (int attempt = 0; attempt < total_attempt; attempt++){
        // Gacha time !
        int item = gacha();

        // Display Gacha result
        if(displayPull){
            if(isGuarantee4star || isGuarantee5star)
                printf("[%d] ", item);
            else if((isSoftPity4star && (item == 4)) || (isSoftPity5star && (item == 5)))
                printf(" %d* ", item);
            else
                printf(" %d  ", item);
            count_printed_item++;
            if(item == 5){
                printf(" ---(%d)\n\n", count_printed_item);
                count_printed_item = 0;
            }
            else if(count_printed_item % 10 == 0){
                printf("\n");
            }
            if(attempt + 1 == total_attempt)
                printf("\n");
        }

        // Record gacha result
        if(item == 5){
            count_5star++;
            if(isGuarantee5star) count_5star_guarantee++;
            else if(isSoftPity5star) count_5star_softPity++;
            else count_5star_notPity++;
        }
        else if(item == 4){
            count_4star++;
            if(isGuarantee4star) count_4star_guarantee++;
            else if(isSoftPity4star) count_4star_softPity++;
            else count_4star_notPity++;
        }
        else
            count_3star++;
    }
    
    // Report
    printf("%d / %d\n",total_roll,total_attempt);
    printf("[5-star character]\n    Total: %d\n Non-pity: %d\nSoft Pity: %d\nGuarantee: %d\n",count_5star,count_5star_notPity,count_5star_softPity,count_5star_guarantee);
    printf("\n");
    printf("[4-star character/weapon]\n    Total: %d\n Non-pity: %d\nSoft Pity: %d\nGuarantee: %d\n",count_4star,count_4star_notPity,count_4star_softPity,count_4star_guarantee);

}

int gacha(){
    int item = randomItemRarity();
    // TODO: random which item of that rarity player will receive

    return item;
}

// return int of item's rarity 3, 4, or 5
int randomItemRarity(){
    wishCountIncrement();
    double rng = genrand();
    double rate_4star = currentRate4star();
    double rate_5star = currentRate5star();
    
    if(rng < rate_5star){
        count_roll_from_last_5star = 0;
        if(isGuarantee5star && isGuarantee4star)
            count_roll_from_last_4star = 0;
        return 5;
    }
    if(rng < (rate_5star + rate_4star)){
        count_roll_from_last_4star = 0;
        return 4;
    }
    return 3;
}

// call every time randomitemRarity() is called.
void wishCountIncrement(){
    total_roll++;
    count_roll_from_last_4star++;
    count_roll_from_last_5star++;
    updatePityStatus();
}

void updatePityStatus(){
    if (count_roll_from_last_5star == guarantee_5star) isGuarantee5star = true; else isGuarantee5star = false;
    if (count_roll_from_last_5star >= softpity_5star)  isSoftPity5star  = true; else isSoftPity5star  = false;
    if (count_roll_from_last_4star == guarantee_4star) isGuarantee4star = true; else isGuarantee4star = false;
    if (count_roll_from_last_4star >= softpity_4star)  isSoftPity4star  = true; else isSoftPity4star  = false;
}

double currentRate5star(){
    if (isGuarantee5star) return 100.0;
    if (isSoftPity5star) return softpity_rate_5star;
    return base_rate_5star;
}

double currentRate4star(){
    if (isGuarantee4star) return 100.0;
    if (isSoftPity4star) return softpity_rate_4star;
    return base_rate_4star;
}



// MT19937 Pseudo RNG by Makoto Matsumoto and Takuji Nishimura
/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df
#define UPPER_MASK 0x80000000
#define LOWER_MASK 0x7fffffff
/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y) (y >> 11)
#define TEMPERING_SHIFT_S(y) (y << 7)
#define TEMPERING_SHIFT_T(y) (y << 15)
#define TEMPERING_SHIFT_L(y) (y >> 18)

static unsigned long mt[N];
static int mti=N+1;

void sgenrand(unsigned long seed){
    mt[0] = seed & 0xffffffff;
    for (mti=1; mti<N; mti++)
        mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

double genrand(){
    unsigned long y;
    static unsigned long mag01[2] = {0x0, MATRIX_A};

    if (mti >= N) {
        int kk;

        if (mti == N+1)   /* if sgenrand() has not been called */
            sgenrand(4357); /* a default initial seed is used */

        for (kk = 0; kk < N - M; kk++) {
            y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
            mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1];
        }

        for (; kk < N - 1; kk++) {
            y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
            mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
        
        mti = 0;
    }

    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return ( (double)y / (unsigned long)0xffffffff ); /* reals */ 
    // return y; /* integer generation; mod y after return because y will have too many digits for int */
}