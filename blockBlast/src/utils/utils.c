#include "utils/utils.h"

u64 randint(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}

u64 prng_randint(u64 min, u64 max) {
    return min + (prng_rand() % (max - min + 1));
}

f64 randfloat(void) {
    return (f64) rand() / RAND_MAX;
}