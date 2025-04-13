#include "../include/vec.h"

void vec_deinit(vec *v) {
    if (v->data) {
        free(v->data);
        v->data = NULL;
    }
    v->elem_size = 0;
    v->len = 0;
    v->capacity = 0;
}

void *_vec_push(vec *v) {
    if (v->capacity == 0) {
        v->capacity = 4;
        v->data = malloc(v->capacity * v->elem_size);
        if (!v->data) {
            return NULL;
        }
    }
    else if (v->len == v->capacity) {
        v->capacity *= 2;
        void *new_data = realloc(v->data, v->capacity * v->elem_size);
        if (!new_data) {
            return NULL;
        }
        v->data = new_data;
    }
    void *new_elem = (char*)v->data + (v->len * v->elem_size);
    v->len++;
    return new_elem;
}

void *_vec_pop(vec *v) {
    v->len--;
    return (char*)v->data + (v->len * v->elem_size);
}

void *_vec_at(vec *v, size_t idx) {
    return (char*)v->data + (idx * v->elem_size);
}

void _vec_remove(vec *v, size_t idx) {
    v->len--;
    if (idx < v->len) {
        memmove((char*)v->data + (idx * v->elem_size),
            (char*)v->data + ((idx + 1) * v->elem_size),
            (v->len - idx) * v->elem_size);
    }
}
