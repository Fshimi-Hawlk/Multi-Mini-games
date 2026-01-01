#ifndef UTILS_H
#define UTILS_H

#include "common.h"

#define INVALID_PTR (void *) -1

#define dcall log_debug("called")

#define signof(v) ((v) < 0 ? -1 : 1)

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define clamp(v, min, max) min(max((min), (v)), (max))

u64 randint(u64 min, u64 max);
f64 randfloat(void); // range 0..=1

#endif // UTILS_H