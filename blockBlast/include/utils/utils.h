#ifndef UTILS_H
#define UTILS_H

#include "common.h"

#define INVALID_PTR (void *) -1

#define dcall log_debug("called")

#define signof(v) ((v) < 0 ? -1 : 1)

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define KiB(n) ((u64) (n) << 10)
#define MiB(n) ((u64) (n) << 20)
#define GiB(n) ((u64) (n) << 30)

#define alignUpPow2(n, p) (((u64) (n) + (u64) (p) - 1) & (~((u64) (p) - 1)))

/*
Xor proprety: 
V(x), x ^ x = 0

a ^= b; // a ^ b
b ^= a; // b ^ (a ^ b) = b ^ b ^ a = 0 ^ a = a
a ^= b; // (a ^ b) ^ a = b ^ a ^ a = b ^ 0 = b
*/
#define swap(a, b) do { a ^= b; b ^= a; a ^= b} while (0)
#define clamp(v, min, max) min(max((min), (v)), (max))

#define vec2Add(v1, v2, T) (T) { .x = (v1).x + (v2).x, .y = (v1).y + (v2).y }
#define vec2Sub(v1, v2, T) (T) { .x = (v1).x - (v2).x, .y = (v1).y - (v2).y }
#define vec2Mul(v1, v2, T) (T) { .x = (v1).x * (v2).x, .y = (v1).y * (v2).y }
#define vec2Div(v1, v2, T) (T) { .x = (v1).x / (v2).x, .y = (v1).y / (v2).y }

#define vec2AddVal(v, val, T) (T) { .x = (v).x + (val), .y = (v).y + (val) }
#define vec2Scale(v, scalar, T) (T) { .x = (v).x * (scalar), .y = (v).y * (scalar) }

#define vec2Fmt(vec2) (vec2).x, (vec2).y
#define vec2fStr "%.2f, %.2f"
#define vec2siStr "%d, %d"
#define vec2uiStr "%u, %u"

#define nl putchar('\n');
#define boolStr(v) (v) ? "true" : "false"

// from nob.h
#ifdef _WIN32
#    define LINE_END "\r\n"
#else
#    define LINE_END "\n"
#endif

#define UNUSED(value) (void)(value)
#define TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))
#define ARRAY_GET(array, index) \
    (assert((size_t)index < ARRAY_LEN(array)), array[(size_t)index])

#define return_defer(value) do { result = (value); goto defer; } while(0)

u64 randint(u64 min, u64 max);
u64 prng_randint(u64 min, u64 max);
f64 randfloat(void); // range 0..=1

#endif // UTILS_H