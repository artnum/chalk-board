#ifndef UTILS_H__
#define UTILS_H__

#include <stdlib.h>
#include <string.h>

void * darray_grow(void * array, size_t * allocated, size_t grow, size_t size);

#endif /* UTILS_H__ */