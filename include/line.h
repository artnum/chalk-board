#ifndef LINE_H__
#define LINE_H__

#include <stdlib.h>
#include <utils.h>
#include <types.h>

#define LINES_ALLOC_CHUNK   256
#define POINTS_ALLOC_CHUNK  1024

void free_lines(struct s_App * app);
struct s_Line * get_next_line(struct s_App * app);
bool add_point_to_line(struct s_Line * line, float x, float y, float pressure);

#define grow_lines(lines, allocated) \
        darray_grow(lines, allocated, LINES_ALLOC_CHUNK, sizeof(struct s_Line))
#define grow_points(points, allocated) \
        darray_grow(points, allocated, POINTS_ALLOC_CHUNK, sizeof(struct s_Point))


#endif /* LINE_H__ */