/*
 * Dynamic Array implementation extracted and adapted from nob.h
 * Original author: Tsoding[](https://github.com/tsoding/nob.h)
 *
 * Original nob.h is dedicated to the public domain (Unlicense):
 * https://github.com/tsoding/nob.h/blob/master/nob.h
 *
 * This is a modified version split into separate headers for personal use.
 * No restrictions apply — you can use, modify, and distribute freely.
 */

#ifndef DYNAMIC_ARRAY_H_
#define DYNAMIC_ARRAY_H_

#include <assert.h>
#include <stdlib.h>

#ifndef ASSERT
#define ASSERT assert
#endif /* ASSERT */

#ifndef REALLOC
#define REALLOC realloc
#endif /* REALLOC */

#ifndef FREE
#define FREE free
#endif /* FREE */

// Initial capacity of a dynamic array
#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif

#define DA(T) \
struct { \
    T *items; \
    size_t count, capacity; \
}

#define typeDA(dataType, daName) \
typedef struct { \
    dataType *items; \
    size_t count, capacity; \
} daName

#define da_reserve(da, expected_capacity)                                                   \
    do {                                                                                    \
        if ((expected_capacity) > (da)->capacity) {                                         \
            size_t new_capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2;   \
            while ((expected_capacity) > new_capacity) {                                    \
                new_capacity *= 2;                                                          \
            }                                                                               \
            (da)->items = REALLOC(                                                          \
                (da)->items,                                                                \
                (da)->capacity * sizeof(*(da)->items),                                      \
                new_capacity * sizeof(*(da)->items)                                         \
            );                                                                              \
            ASSERT((da)->items != NULL);                                                    \
            (da)->capacity = new_capacity;                                                  \
        }                                                                                   \
    } while (0)

// Append an item to a dynamic array
#define da_append(da, item)                \
    do {                                       \
        da_reserve((da), (da)->count + 1); \
        (da)->items[(da)->count++] = (item);   \
    } while (0)

#define da_free(da) FREE((da).items)

// Append several items to a dynamic array
#define da_append_many(da, new_items, new_items_count)                                      \
    do {                                                                                        \
        da_reserve((da), (da)->count + (new_items_count));                                  \
        memcpy((da)->items + (da)->count, (new_items), (new_items_count)*sizeof(*(da)->items)); \
        (da)->count += (new_items_count);                                                       \
    } while (0)

#define da_resize(da, new_size)     \
    do {                                \
        da_reserve((da), new_size); \
        (da)->count = (new_size);       \
    } while (0)

#define da_last(da) (da)->items[(ASSERT((da)->count > 0), (da)->count-1)]

/**
  *  Remove the i-th item of the dynamic array without conserving order.
  */
#define da_remove_unordered(da, i)               \
    do {                                             \
        size_t j = (i);                              \
        ASSERT(j < (da)->count);                 \
        (da)->items[j] = (da)->items[--(da)->count]; \
    } while(0)

// Foreach over Dynamic Arrays. Example:
// ```c
// DA(int) xs = {0};
//
// da_append(&xs, 69);
// da_append(&xs, 420);
// da_append(&xs, 1337);
//
// da_foreach(int, x, &xs) {
//     // `x` here is a pointer to the current element. You can get its index by taking a difference
//     // between `x` and the start of the array which is `x.items`.
//     size_t index = x - xs.items;
// }
// ```
#define da_foreach(Type, it, da) for (Type *it = (da)->items; it < (da)->items + (da)->count; ++it)

#endif // DYNAMIC_ARRAY_H_