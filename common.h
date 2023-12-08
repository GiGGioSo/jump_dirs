#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <assert.h>
#include <string.h>

#define return_defer(ret) do { result = ret; goto defer; } while(0)

#define ARRAY_LENGTH(arr) ((int)(sizeof(arr) / sizeof(arr[0])))

#define UNUSED(expr) (void)(expr)

#define POW2(x) ((x) * (x))

#define DA_INITIAL_CAPACITY 32

#define da_append(da, item)                                               \
do {                                                                      \
    if ((da)->count >= (da)->capacity) {                                  \
        (da)->capacity = ((da)->capacity == 0) ?                          \
            DA_INITIAL_CAPACITY : (da)->capacity*2;                       \
        (da)->items = realloc((da)->items,                                \
                              (da)->capacity*sizeof(*da->items));         \
        assert((da)->items != NULL && "Buy more RAM lol");                \
    }                                                                     \
    (da)->items[(da)->count++] = (item);                                  \
} while (0)

#define da_remove(da, index)                                              \
do {                                                                      \
    size_t type_size = sizeof(*(da)->items);                              \
    memmove((uint8_t *)((da)->items) + (index) * type_size,               \
            (uint8_t *)((da)->items) + ((index)+1) * type_size,           \
            ((da)->count - (index) - 1) * type_size);                     \
    (da)->count--;                                                        \
} while (0)

#define da_clear(da)                                                      \
do {                                                                      \
    if ((da)->items) free((da)->items);                                   \
    (da)->items = NULL;                                                   \
    (da)->count = 0;                                                      \
    (da)->capacity = 0;                                                   \
} while (0)

#define da_swap(da, i, j, T)                                              \
do {                                                                      \
    if (i >= (da)->count || j >= (da)->count) {                           \
        printf("[ERROR] Accessing element %d from array of length %d",    \
               ((i > j) ? i : j), (da)->count);                           \
    }                                                                     \
    T tmp = (da)->items[i];                                               \
    (da)->items[i] = (da)->items[j];                                      \
    (da)->items[j] = tmp;                                                 \
} while (0)

#define da_last(da) ((da)->items[(da)->count-1])

#endif // COMMON_H_
