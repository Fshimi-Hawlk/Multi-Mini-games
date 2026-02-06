#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils/utils.h"
#include "utils/utils.c"

s32 main(void) {
    printf("Running tests for utils/utils...\n");

    srand(time(NULL));

    // Test randint
    {
        u64 min = 1, max = 10;
        u64 val = randint(min, max);
        assert(val >= min && val <= max && "randint in range");
    }

    // Test randfloat
    {
        f64 val = randfloat();
        assert(val >= 0.0 && val <= 1.0 && "randfloat in [0,1]");
    }

    // Macros like min, max, swap, clamp (test a few)
    {
        assert(min(5, 10) == 5 && "min macro");
        assert(max(5, 10) == 10 && "max macro");

        int a = 1, b = 2;
        swap(a, b);
        assert(a == 2 && b == 1 && "swap macro");

        assert(clamp(15, 0, 10) == 10 && "clamp macro upper");
        assert(clamp(-5, 0, 10) == 0 && "clamp macro lower");
    }

    printf("All tests passed for utils/utils!\n");
    return 0;
}