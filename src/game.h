#ifndef GAME_H
#define GAME_H

#define BOARD_WIDTH 700
#define BOARD_HEIGHT 500
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
/* Avoid trouble with --std=c99*/
#define M_PI 3.14159265358979323846264338327
#define BOARD_X ((SCREEN_WIDTH - BOARD_WIDTH) / 2)
#define BOARD_Y ((SCREEN_HEIGHT - BOARD_HEIGHT) / 2)

int doGame(SDL_Renderer *, SDL_bool);

#endif
