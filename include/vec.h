#ifndef VEC_H
#define VEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t elem_size;
    size_t capacity;
    size_t len;
    void *data;
} vec;

#define vec_init(type) (vec){ sizeof(type), 0, 0, 0 }
void vec_deinit(vec *v);

void *_vec_push(vec *v);
void *_vec_pop(vec *v);
void *_vec_at(vec *v, size_t idx);
void _vec_remove(vec *v, size_t idx);

#define vec_remove(v, idx) _vec_remove(v, idx)

#define vec_set(v, idx, name) (*((typeof(name)*)(_vec_at(v, idx))) = name)
#define vec_get(name, v, idx) name = ((typeof (name))_vec_at(v, idx))
#define vec_pop(name, v) name = ((typeof (name))_vec_pop(v))
#define vec_push(v, name) (*((typeof(name)*)(_vec_push(v))) = name)

#endif
