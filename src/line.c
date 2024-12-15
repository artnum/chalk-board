#include <SDL3/SDL_log.h>
#include <chalk-board.h>
#include <line.h>

void free_lines(struct s_App * app)
{
    if (app == NULL) { return; }
    if (app->lines == NULL) { return; }

    for (size_t i = 0; i < app->allocated; i++) {
        free(app->lines[i].points);
    }
    free(app->lines);
}

struct s_Line * get_next_line(struct s_App * app)
{
    for (size_t i = 0; i < app->allocated; i++) {
        if (!app->lines[i].used) {
            app->lines[i].used = true;
            return &app->lines[i];
        }
    }
    size_t new_size = app->allocated;
    struct s_Line * new_ptr = NULL;
    new_ptr = grow_lines(app->lines, &new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    app->lines = (struct s_Line *)new_ptr;
    for(size_t i = app->allocated; i < new_size; i++) {
        app->lines[i].last_point = 0;
        app->lines[i].used = false;
        app->lines[i].allocated = 0;
        app->lines[i].points = NULL;
    }
    app->lines[app->allocated].used = true;
    app->last_line = app->allocated;
    app->allocated = new_size;
    return &app->lines[app->last_line];
}

bool add_point_to_line(struct s_Line * line, float x, float y, float pressure)
{
    for (size_t i = line->last_point; i < line->allocated; i++) {
        if (!line->points[i].used) {
            if (i == 0) {
                /* init random with some value, randomness is used for coolness
                 * so if it is not really random we don't care.
                 */
                line->rand_init = (unsigned int) x ^ (unsigned int) y ^ *(unsigned int *)line;
            }
            line->last_point = i;
            line->points[i].used = true;
            line->points[i].x = x;
            line->points[i].y = y;
            line->points[i].pressure = pressure;
            return true;
        }
    }

    size_t new_size = line->allocated;
    struct s_Point * new_ptr = NULL;
    new_ptr = grow_points(line->points, &new_size);
    if (new_ptr == NULL) {
        return false;
    }
    SDL_Log("new_size %ld previous size %ld", new_size, line->allocated);
    for (size_t i = line->allocated; i < new_size; i++) {
        new_ptr[i].used = false;
        new_ptr[i].x = 0.0f;
        new_ptr[i].y = 0.0f;
    }
    line->points = new_ptr;
    line->last_point = line->allocated;
    line->allocated = new_size;
    line->points[line->last_point].used = true;
    line->points[line->last_point].x = x;
    line->points[line->last_point].y = y;
    line->points[line->last_point].pressure = pressure;
    if (line->last_point == 0) {
        line->rand_init = (unsigned int) x ^ (unsigned int) y ^ *(unsigned int *)line;
    }
    return true;
}