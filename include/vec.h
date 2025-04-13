#ifndef VEC_H
#define VEC_H

#include <stdlib.h>

#define VEC_ZERO() { 0, 0 }

#define DEFINE_VEC(type) \
    typedef struct { \
        type *data; \
        int n; \
    } type ## _vec; \
    void type ## _vec_push(type ## _vec vec, type value) { \
        vec.data = (type*)realloc(vec.data, (++vec.n) * sizeof(type)); \
        vec.data[vec.n - 1] = value; \
    } \
    type type ## _vec_pop(type ## _vec vec) { \
        return vec.data[--vec.n]; \
    }

#define VEC_FOR_EACH(vec, idx, name, code) \
    for (int idx = 0; idx < vec.n; idx++) { \
         typeof(*vec.data) name = vec.data[idx]; \
         code \
    }

#define ARRAY_FOR_EACH(arr, idx, name, code) \
    for (int idx = 0; idx < sizeof arr / sizeof *arr; idx++) { \
        typeof(*arr) name = arr[idx];\
        code; \
    } \

#endif
