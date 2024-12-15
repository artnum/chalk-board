#include <utils.h>

void * darray_grow(void * array, size_t * allocated, size_t grow, size_t size)
{
    array = realloc(array, (*allocated + grow) * size);
    if (array == NULL) {
        return NULL;
    }
    *allocated = *allocated + grow;
    return array;
}