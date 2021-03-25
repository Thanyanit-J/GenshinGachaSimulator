//! Genshin Gacha Simulator
//! 
//! The information of probability is based on a game Genshin Impact https://genshin.mihoyo.com/en
//! The information of soft-pity probability is from https://bbs.mihoyo.com/ys/article/2102609
//! The MT19937 pseudo RNG code by Makoto Matsumoto and Takuji Nishimura is from http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/ARTICLES/mt.pdf
//!

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constant Variables  (Feel free to adjust any variables in this section)
const double base_rate_5star = 0.006;     // 0.6% = 0.006
const double softpity_rate_5star = 0.324;
const int guarantee_5star = 90;
const int softpity_5star = 76;

const double base_rate_4star = 0.051;    // 5.1% = 0.051
const double softpity_rate_4star = 0.511;
const int guarantee_4star = 10;
const int softpity_4star = 9;

const int total_attempt = 100000000;

const bool displayPull = false;

// Function Prototypes
int gacha();
int randomItemRarity();
void wishCountIncrement();
void updatePityStatus();
double currentRate5star();
double currentRate4star();
void gachaDisplay(int item);
void gachaRecord(int item);
void gachaReport();
int countDigit(int n);

// RNG in this code is Mersenne Twister pseudo random number generator by Makoto Matsumoto and Takuji Nishimura
void sgenrand(unsigned long seed);
double genrand();

// Global Variables
unsigned int count_roll_from_last_4star = 0;
unsigned int count_roll_from_last_5star = 0;
int total_roll = 0;
bool isGuarantee5star = false;
bool isGuarantee4star = false;
bool isSoftPity5star = false;
bool isSoftPity4star = false;

int total_5star_item = 0;
unsigned int total_5star_item_nonPity = 0;
unsigned int total_5star_item_softPity = 0;
unsigned int total_5star_item_guarantee = 0;

unsigned int total_4star_item = 0;
unsigned int total_4star_item_nonPity = 0;
unsigned int total_4star_item_softPity = 0;
unsigned int total_4star_item_guarantee = 0;

unsigned int total_3star = 0;

unsigned int total_5star_nonPity = 0;
unsigned int total_5star_softPity = 0;
unsigned int total_4star_nonPity = 0;
unsigned int total_4star_softPity = 0;

unsigned int count_printed_item = 0;

///////////////////////////////////// MAIN /////////////////////////////////////

int main(int argc, char *argv[]) {
    // Using current machine time as a seed for RNG function.
    sgenrand(time(0));

    for (int attempt = 0; attempt < total_attempt; attempt++){
        // Gacha time !
        int item = gacha();

        gachaDisplay(item);
        gachaRecord(item);
    }
    
    gachaReport();
}

/////////////////////////////////// END MAIN ///////////////////////////////////

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

void gachaDisplay(int item){
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
    }
}

void gachaRecord(int item){
    // Count items
    if (item == 5) {
        total_5star_item++;
        if(isGuarantee5star) total_5star_item_guarantee++;
        else if(isSoftPity5star) total_5star_item_softPity++;
        else total_5star_item_nonPity++;
    }
    else if (item == 4) {
        total_4star_item++;
        if(isGuarantee4star) total_4star_item_guarantee++;
        else if(isSoftPity4star) total_4star_item_softPity++;
        else total_4star_item_nonPity++;
    }
    else 
        total_3star++;

    // Count total pity/nonpity
    if (!isGuarantee5star && !isSoftPity5star) total_5star_nonPity++;
    else if(!isGuarantee5star && isSoftPity5star) total_5star_softPity++;

    if (!isGuarantee4star && !isSoftPity4star) total_4star_nonPity++;
    else if (!isGuarantee4star && isSoftPity4star) total_4star_softPity++;
}

void gachaReport(){
    int width = countDigit(total_roll);
    printf("\nSuccessfully simulated %d out of %d Wishes.\n\n", total_roll,total_attempt);
    printf("[5-star Drops]\n");
    printf("     Total: %*d\t\tConsolidated probability (incl. guarantee): %.3lf%%\n", width, total_5star_item, 100.0*(double)total_5star_item/(double)total_attempt);
    printf("  Non-pity: %*d\t\tProbability per non-pity roll: %.3lf%%\n", width, total_5star_item_nonPity, 100.0*(double)total_5star_item_nonPity/(double)total_5star_nonPity);
    printf(" Soft Pity: %*d\t\tProbability per soft-pity roll: %.3lf%%\n", width,total_5star_item_softPity, 100.0*(double)total_5star_item_softPity/(double)total_5star_softPity);
    printf(" Guarantee: %*d\n", width, total_5star_item_guarantee);
    printf("\n");
    printf("[4-star Drops]\n");
    printf("     Total: %*d\t\tConsolidated probability (incl. guarantee): %.3lf%%\n", width, total_4star_item, 100.0*(double)total_4star_item/(double)total_attempt);
    printf("  Non-pity: %*d\t\tProbability per non-pity roll: %.3lf%%\n", width, total_4star_item_nonPity, 100.0*(double)total_4star_item_nonPity/(double)total_4star_nonPity);
    printf(" Soft Pity: %*d\t\tProbability per soft-pity roll: %.3lf%%\n", width, total_4star_item_softPity, 100.0*(double)total_4star_item_softPity/(double)total_4star_softPity);
    printf(" Guarantee: %*d\n", width, total_4star_item_guarantee);
    printf("\n");
}

int countDigit(int n){
    int digit = 0;
    if (n < 0) n*= -1;
    while(n / 10 > 0) {
        digit++;
        n /= 10;
    }
    return digit;
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