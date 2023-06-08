/** TODO: Use #ifdef guards, though this may be impossible */

#define concat(a, b) a##b
#define typename(T) concat(T, List)
#define tricat(a, b, c) a##b##c
#define funcname(type, func) tricat(type, _, func)

#include <stddef.h>
#include <stdbool.h>

typedef struct typename(T) typename(T);

typename(T) *funcname(typename(T), new)(size_t initcap);

void funcname(typename(T), delete)(typename(T) **self);

size_t funcname(typename(T), size)(const typename(T) *self);

const T *funcname(typename(T), begin)(const typename(T) *self);

const T *funcname(typename(T), end)(const typename(T) *self);

T funcname(typename(T), get)(const typename(T) *self, size_t i);

bool funcname(typename(T), push)(typename(T) **self, T value);

void funcname(typename(T), clear)(typename(T) *self);

bool funcname(typename(T), copy)(const typename(T) *restrict self, typename(T) *restrict *copyee);

#undef concat
#undef typename
#undef tricat
#undef funcname
#undef T
