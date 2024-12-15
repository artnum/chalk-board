#ifndef CHALK_BOARD_H__
#define CHALK_BOARD_H__

#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <line.h>

#include <types.h>

void white_chalk(SDL_Renderer * r, float alpha);
void red_chalk(SDL_Renderer * r, float alpha);
void blue_chalk(SDL_Renderer * r, float alpha);
void green_chalk(SDL_Renderer * r, float alpha);
void yellow_chalk(SDL_Renderer * r, float alpha);
void pink_chalk(SDL_Renderer * r, float alpha);
void orange_chalk(SDL_Renderer * r, float alpha);

#endif /* CHALK_BOARD_H__ */