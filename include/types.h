#ifndef TYPES_H__ 
#define TYPES_H__

#include <SDL3/SDL.h>

enum e_Pos {
    POS_X,
    POS_Y,
    POS__MAX__
};

enum e_ChalkColor {
    WHITE,
    RED,
    BLUE,
    GREEN,
    YELLOW,
    PINK,
    ORANGE,
    COLOR__MAX__
};


struct s_Point {
    bool used;
    float x;
    float y;
    float pressure;
};

struct s_Line {
    unsigned int rand_init;
    size_t last_point;
    bool used;
    struct s_Point * points;
    size_t allocated;
    float thickness;
    void (*chalk)(SDL_Renderer * r, float alpha);

};


struct s_App {
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * renderer_target;
    SDL_Texture * erase_texture;
    enum e_ChalkColor color;
    bool touching;
    bool fullscreen;
    bool erase;
    bool updated;
    float thickness;
    float pressure;
    float position[POS__MAX__];
    float previous_position[POS__MAX__];
    void (*chalk[COLOR__MAX__])(SDL_Renderer * r, float alpha);

    /* TODO make a draw structure, move this into draw struct add possibility
     * to have a dynamic array of struct draw.
     */
    struct s_Line * lines;
    struct s_Line * current_line;
    size_t allocated;
    size_t last_line;
    
};

#endif /* TYPES_H__ */