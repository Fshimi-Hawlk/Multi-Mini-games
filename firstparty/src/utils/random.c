#include "utils/random.h"
#include <stdlib.h>

u64 randint(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}

#ifdef RAND_H

u64 prng_randint(u64 min, u64 max) {
    return min + (prng_rand() % (max - min + 1));
}

#endif

f64 randfloat(void) {
    return (f64) rand() / RAND_MAX;
}