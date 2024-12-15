#include "line.h"
#include <chalk-board.h>

#include <stdlib.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_pen.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#define WIDTH 1920
#define HEIGHT 1080

void white_chalk(SDL_Renderer * r, float alpha)
{
    SDL_SetRenderDrawColorFloat(r, 1.0f, 1.0f, 1.0f, alpha);
}

void red_chalk(SDL_Renderer * r, float alpha)
{
    SDL_SetRenderDrawColorFloat(r, 1.0f, 0, 0, alpha);
}

void blue_chalk(SDL_Renderer * r, float alpha)
{
    SDL_SetRenderDrawColorFloat(r, 0, 0, 1.0f, alpha);
}

void green_chalk(SDL_Renderer * r, float alpha)
{
    SDL_SetRenderDrawColorFloat(r, 0, 1.0f, 0, alpha);
}

void yellow_chalk(SDL_Renderer * r, float alpha)
{
    SDL_SetRenderDrawColorFloat(r, 1.0f, 1.0f, 0, alpha);
}

void pink_chalk(SDL_Renderer * r, float alpha)
{
    SDL_SetRenderDrawColorFloat(r, 1.0f, 0.713f, 0.756f, alpha);
}

void orange_chalk(SDL_Renderer * r, float alpha)
{
    SDL_SetRenderDrawColorFloat(r, 1.0f, 0.647f, 0.0f, alpha);
}

SDL_AppResult SDL_AppInit(void ** appstate, int argc, char ** argv)
{

    struct s_App * app = NULL;
    *appstate = calloc(1, sizeof(*app));
    if (*appstate == NULL) {
        SDL_Log("Init calloc failed");
        return SDL_APP_FAILURE;
    }

    app = (struct s_App *)*appstate;
    app->chalk[WHITE] = white_chalk;
    app->chalk[RED] = red_chalk;
    app->chalk[PINK] = pink_chalk;
    app->chalk[BLUE] = blue_chalk;
    app->chalk[GREEN] = green_chalk;
    app->chalk[YELLOW] = yellow_chalk;
    app->chalk[ORANGE] = orange_chalk;
    app->color = WHITE;
    app->erase = false;
    app->fullscreen = false;
    app->updated = false;
    app->thickness = 3.0f;
    app->pressure = 0.0f;
    app->previous_position[POS_X] = -1.0f;
    app->previous_position[POS_Y] = -1.0f;

    SDL_SetAppMetadata("BBoard", "1.0", "ch.artnum.bboard");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL Init failed <%s>", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("BBoard", WIDTH, HEIGHT, 0, &app->window, &app->renderer)) {
        SDL_Log("Window and renderer failed <%s>", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    app->renderer_target = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    if (!app->renderer_target) {
        SDL_Log("Failed Create texture target <%s>", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    app->erase_texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    if (!app->erase_texture) {
        SDL_Log("Failed Create texture target <%s>", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_HideCursor();

    SDL_SetTextureBlendMode(app->erase_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(app->renderer_target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(app->renderer, app->renderer_target);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);

    SDL_SetRenderTarget(app->renderer, app->erase_texture);
    SDL_SetRenderDrawColor(app->renderer, 127, 127, 127, SDL_ALPHA_TRANSPARENT);
    SDL_RenderClear(app->renderer);

    SDL_SetRenderTarget(app->renderer, NULL);
    
    return SDL_APP_CONTINUE;
}


static inline void swap(float * a, float * b) {
    float x = *a;
    *a = *b;
    *b = x;
}

void clear_erase_texture(struct s_App * app) {
    SDL_SetRenderTarget(app->renderer, app->erase_texture);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
    SDL_RenderClear(app->renderer);
    app->updated = false;
}

void draw_thick_line (
    struct s_App * app,
    float x0,
    float y0,
    float x1,
    float y1,
    float thick,
    float press,
    void (*chalk)(SDL_Renderer * r, float alpha)
)
{
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    SDL_SetRenderTarget(app->renderer, app->renderer_target);
    const float thickness = thick;
    do {
        float p = press / thickness;
        for (float t = thickness; t > 0.0f; t -= (thickness / 3.0f) * (float)(rand() / (float)RAND_MAX)) {
            p += p;
            for (float alpha = 0.0; alpha <= SDL_PI_F * 2; alpha += (float)(rand() / (float)RAND_MAX)) {

                float xbis0 = x0 +  t * SDL_cosf(alpha);
                float ybis0 = y0 + t  * SDL_sinf(alpha);
                
                xbis0 += (float)(rand() / (float)RAND_MAX) * t;
                ybis0 += (float)(rand() / (float)RAND_MAX) * t;
                chalk(app->renderer, (float)(rand() / (float)RAND_MAX) * p);
                SDL_RenderPoint(app->renderer, xbis0, ybis0);
            }
        }
        float dx = x1 - x0;
        float dy = y1 - y0;
        float hypo = SDL_sqrtf(dx * dx + dy * dy);
        if (thickness / 2 > hypo) {
            break;
        }
        float angle = SDL_atan2f(dy, dx);
        x0 = x0 + (hypo / thickness) * SDL_cosf(angle);
        y0 = y0 + (hypo / thickness) * SDL_sinf(angle);
    } while (1);
}

void draw_last_point (struct s_App * app, struct s_Line * line)
{
    draw_thick_line(
            app,
            line->points[line->last_point - 1].x,
            line->points[line->last_point - 1].y,
            line->points[line->last_point].x,
            line->points[line->last_point].y,
            line->thickness,
            line->points[line->last_point - 1].pressure + line->points[line->last_point].pressure / 2,
            line->chalk
        );
}

void draw_line(struct s_App * app, struct s_Line * line)
{
    struct s_Point * previous = NULL;
    srand(line->rand_init);
    if (line->points == NULL || line->allocated <= 1) { return; }
    if (!line->points[0].used) { return; }

    previous = &line->points[0];
    for (size_t i = 1; i < line->allocated; i++) {
        if (!line->points[i].used) { break; }
        draw_thick_line(
            app,
            previous->x,
            previous->y,
            line->points[i].x,
            line->points[i].y,
            line->thickness,
            previous->pressure + line->points[i].pressure / 2,
            line->chalk
        );
        previous = &line->points[i];

    }
}

void handle_motion (struct s_App * app, float x, float y, float pressure)
{
    if (app->erase) {
        SDL_FRect rect = {
            .x = x - app->thickness * 10 / 2 + 1,
            .y = y - app->thickness * 10 / 2 + 1,
            .w = app->thickness * 10 + 1,
            .h = app->thickness * 10 + 1
        };
        SDL_SetRenderTarget(app->renderer, app->erase_texture);
        SDL_SetRenderDrawColor(app->renderer, 127, 127, 127, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(app->renderer);
        SDL_SetRenderDrawColorFloat(app->renderer, 0.5f, 0.5f, 0.5f, SDL_ALPHA_OPAQUE);
        SDL_RenderRect(app->renderer, &rect);
        if (app->touching) {
            rect.x--;
            rect.y--;
            rect.w--;
            rect.h--;
            SDL_SetRenderTarget(app->renderer, app->renderer_target);
            SDL_SetRenderDrawColorFloat(app->renderer, 0.0f, 0.0f, 0.0f, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(app->renderer, &rect);
        }
        app->updated = false;
        return;
    }
    if (pressure <= 0.0f) { return; }
    if (app->previous_position[POS_X] >= 0.0f && app->touching) {
        add_point_to_line(
            app->current_line,
            x,
            y,
            pressure
        );
        draw_last_point (app, app->current_line);
        app->updated = false;
    }
    app->previous_position[POS_X] = x;
    app->previous_position[POS_Y] = y;    
}

void handle_down(struct s_App * app, float x, float y, float pressure)
{
    app->touching = true;
    app->current_line = get_next_line(app);
    app->current_line->thickness = app->thickness;
    app->current_line->chalk = app->chalk[app->color];
    if (!add_point_to_line(
        app->current_line,
        x,
        y,
        pressure
    )) {
        SDL_Log("Failed addition of point");
    }
    srand(app->current_line->rand_init);
}

SDL_AppResult SDL_AppEvent(void * appstate, SDL_Event * event)
{
    struct s_App * app = appstate;
    if (app == NULL) {
        return SDL_APP_FAILURE;
    }
    switch (event->type) {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS;
            break;
        }

        case SDL_EVENT_KEY_UP: {
            switch(event->key.key) {
                default: break;
                case SDLK_1: {
                    app->color = WHITE;
                    if (app->erase) {
                        app->erase = false;
                        clear_erase_texture(app);
                    }
                    break;
                }
                case SDLK_2: {
                    app->color = RED;
                    if (app->erase) {
                        app->erase = false;
                        clear_erase_texture(app);
                    }
                    break;
                }
                case SDLK_3: {
                    app->color = GREEN;
                    if (app->erase) {
                        app->erase = false;
                        clear_erase_texture(app);
                    }
                    break;
                }
                case SDLK_4: {
                    app->color = BLUE;
                    if (app->erase) {
                        app->erase = false;
                        clear_erase_texture(app);
                    }
                    break;
                }
                case SDLK_5: {
                    app->color = YELLOW;
                    if (app->erase) {
                        app->erase = false;
                        clear_erase_texture(app);
                    }
                    break;
                }
                case SDLK_6: {
                    app->color = ORANGE;
                    if (app->erase) {
                        app->erase = false;
                        clear_erase_texture(app);
                    }
                    break;
                }
                case SDLK_7: {
                    app->color = PINK;
                    if (app->erase) {
                        app->erase = false;
                        clear_erase_texture(app);

                    }
                    break;
                }
                case SDLK_0: {
                    if (!app->erase) {
                        SDL_FRect rect = {
                            .x = app->previous_position[POS_X] - app->thickness * 10 / 2 + 1,
                            .y = app->previous_position[POS_Y] - app->thickness * 10 / 2 + 1,
                            .w = app->thickness * 10 + 1,
                            .h = app->thickness * 10 + 1
                        };
                        SDL_SetRenderTarget(app->renderer, app->erase_texture);
                        SDL_SetRenderDrawColor(app->renderer, 127, 127, 127, SDL_ALPHA_TRANSPARENT);
                        SDL_RenderClear(app->renderer);
                        SDL_SetRenderDrawColorFloat(app->renderer, 0.5f, 0.5f, 0.5f, SDL_ALPHA_OPAQUE);
                        SDL_RenderRect(app->renderer, &rect);
                    }
                    app->erase = true;
                    app->updated = false;
                    break;
                }
                case SDLK_F1: {
                    app->thickness = 1.0f;
                    if (app->erase) { app->updated = false; }
                    break;
                }
                case SDLK_F2: {
                    app->thickness = 5.0f;
                    if (app->erase) { app->updated = false; }
                    break;
                }
                case SDLK_F3: {
                    app->thickness = 8.0f;
                    if (app->erase) { app->updated = false; }
                    break;
                }
                case SDLK_F4: {
                    app->thickness = 12.0f;
                    if (app->erase) { app->updated = false; }
                    break;
                }
                case SDLK_F5: {
                    app->thickness = 15.0f;
                    if (app->erase) { app->updated = false; }
                    break;
                }
                case SDLK_F6: {
                    app->thickness = 20.0f;
                    if (app->erase) { app->updated = false; }
                    break;
                }
                case SDLK_F7: {
                    app->thickness = 40.0f;
                    if (app->erase) { app->updated = false; }
                    break;
                }
                case SDLK_F11: {
                    app->fullscreen = !app->fullscreen;
                    SDL_SetWindowFullscreen(app->window, app->fullscreen);
                    app->updated = false;
                    break;
                }
                case SDLK_DELETE: {
                    app->erase = false;
                    clear_erase_texture(app);
                    SDL_SetRenderTarget(app->renderer, app->renderer_target);
                    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                    SDL_RenderClear(app->renderer);
                    app->updated = false;
                    break;
                }
                case SDLK_F12: {
                    for (size_t i = 0; i < app->allocated; i++) {
                        if (!app->lines[i].used) {
                            break;
                        }

                        draw_line(app, &app->lines[i]);
                    }
                }
            }
            break;
        }

        case SDL_EVENT_MOUSE_MOTION: {
            handle_motion(app, event->motion.x, event->motion.y, 1.0f);
            break;
        }
        case SDL_EVENT_PEN_MOTION : {
            handle_motion(app, event->pmotion.x, event->pmotion.y, app->pressure);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if (event->button.button == SDL_BUTTON_LEFT) {
                handle_down(app, event->button.x, event->button.y, 1.0f);
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP: {
            if (event->button.button == SDL_BUTTON_LEFT) {
                if (app->touching) {
                    app->current_line = NULL;

                }
                app->touching = false;
            }
        break;
        }

        case SDL_EVENT_PEN_DOWN: {
            handle_down(app, event->pmotion.x, event->pmotion.y, app->pressure);
            break;
        }
        case SDL_EVENT_PEN_UP: {
            if (app->touching) {
                app->current_line = NULL;
        
            }
            app->touching = false;
            break;
        }

        case SDL_EVENT_PEN_AXIS: {
            if (event->paxis.axis == SDL_PEN_AXIS_PRESSURE && app->touching) {
                app->pressure = event->paxis.value;
            }
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void * appstate) 
{
    struct s_App * app = appstate;
    //if (app->updated) { SDL_Delay(5); return SDL_APP_CONTINUE; }

    SDL_SetRenderTarget(app->renderer, NULL);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);
    SDL_RenderTexture(app->renderer, app->renderer_target, NULL, NULL);
    if (app->erase) { 
        SDL_RenderTexture(app->renderer, app->erase_texture, NULL, NULL);
    }
    SDL_RenderPresent(app->renderer);
    app->updated = true;
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void * appstate, SDL_AppResult result) 
{
    struct s_App * app = appstate;
    SDL_DestroyTexture(app->renderer_target);
    free_lines(app);
    free(app);
} 