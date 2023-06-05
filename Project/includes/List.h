/** TODO: Figure out how to add a guard clause for this file; it may not be possible */

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

#define concat(a, b) a##b
#define typename(t) concat(t, List)
#define tricat(a, b, c) a##b##c
#define funcname(prefix, name) tricat(prefix, _, name)

typedef struct typename(T) typename(T);

typename(T) *funcname(typename(T), new)(size_t initcap);

void funcname(typename(T), delete)(typename(T) **self);

size_t funcname(typename(T), size)(const typename(T) *self);

T funcname(typename(T), get)(const typename(T) *self, size_t i);

const T *funcname(typename(T), getptr)(const typename(T) *self, size_t i);

const T *funcname(typename(T), begin)(const typename(T) *self);

const T *funcname(typename(T), end)(const typename(T) *self);

bool funcname(typename(T), push)(typename(T) *self, T value);

void funcname(typename(T), println)(const typename(T) *self, void (*print)(T value));

void funcname(typename(T), shrink)(typename(T) *self, size_t newsize);

#undef concat
#undef typename
#undef tricat
#undef funcname

#undef T
