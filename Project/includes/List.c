#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "ListPreamble.h"

#define concat(a, b) a##b
#define typename(t) concat(t, List)
#define tricat(a, b, c) a##b##c
#define funcname(prefix, name) tricat(prefix, _, name)

typedef struct typename(T) {
    size_t size, cap;
    T *data;
} typename(T);

typename(T) *funcname(typename(T), new)(size_t initcap)
{
    assert(initcap > 0);

    typename(T) *ret = malloc(sizeof *ret);

    if (ret == NULL)
        return NULL;

    ret->size = 0;
    ret->cap = initcap;
    ret->data = malloc(initcap * sizeof *ret->data);
    
    if (ret->data == NULL) {
        free(ret);
        return NULL;
    }

    return ret;
}

void funcname(typename(T), delete)(typename(T) **self)
{
    assert(self != NULL && *self != NULL);
    assert((*self)->data != NULL);

    free((*self)->data);
    free(*self);
    *self = NULL;
}

size_t funcname(typename(T), size)(const typename(T) *self)
{
    assert(self != NULL);

    return self->size;
}

T funcname(typename(T), get)(const typename(T) *self, size_t i)
{
    assert(self != NULL);
    assert(self->data != NULL);
    assert(i < self->size);

    return self->data[i];
}

const T *funcname(typename(T), getptr)(const typename(T) *self, size_t i)
{
    assert(self != NULL);
    assert(self->data != NULL);
    assert(i < self->size);

    return self->data + i;
}

const T *funcname(typename(T), begin)(const typename(T) *self)
{
    assert(self != NULL);
    assert(self->data != NULL);
    assert(self->size > 0);

    return self->data;
}

const T *funcname(typename(T), end)(const typename(T) *self)
{
    assert(self != NULL);
    assert(self->data != NULL);

    return self->data + self->size;
}

static bool resize(typename(T) *self)
{
    assert(self != NULL);
    assert(self->data != NULL);
    assert(self->cap > 0);

    T *temp = realloc(self->data, self->cap * 2 * sizeof *self->data);

    if (temp == NULL)
        return false;

    self->data = temp;
    self->cap *= 2;

    return true;
}

bool funcname(typename(T), push)(typename(T) *self, T value)
{
    assert(self != NULL);
    assert(self->data != NULL);

    if (self->size + 1 > self->cap)
        if (!resize(self))
            return false;

    self->data[self->size] = value;
    self->size++;

    return true;
}

void funcname(typename(T), println)(const typename(T) *self, void (*print)(T value))
{
    assert(self != NULL);
    assert(self->data != NULL);

    printf("(%zu) [ ", self->size);

    for (size_t i = 0; i < self->size; i++) {
        print(self->data[i]);
        putchar(' ');
    }

    puts("]");
}

void funcname(typename(T), shrink)(typename(T) *self, size_t newsize)
{
    assert(self != NULL);
    assert(newsize <= self->size);

    self->size = newsize;
}
