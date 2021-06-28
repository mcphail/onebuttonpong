#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "colours.h"
#include "title.h"
#include "game.h"
#include "sounds.h"
#include "text.h"

static void cleanup(void);

static SDL_Window *mainWindow = NULL;
static SDL_Renderer *mainRenderer = NULL;

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
		fprintf(stderr, "SDL Initialization Failed: %s\n",
		        SDL_GetError());
		return 1;
	}
	atexit(cleanup);

#ifdef WINDOWED
	int rv = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT,
	                                     SDL_WINDOW_INPUT_FOCUS,
	                                     &mainWindow, &mainRenderer);
	if (rv || !mainWindow || !mainRenderer) {
		fprintf(stderr, "Couldn't create SDL window or renderer: %s\n",
		        SDL_GetError());
		exit(EXIT_FAILURE);
	}
#else
	int rv = SDL_CreateWindowAndRenderer(0, 0,
	                                     SDL_WINDOW_FULLSCREEN_DESKTOP,
	                                     &mainWindow, &mainRenderer);
	if (rv || !mainWindow || !mainRenderer) {
		fprintf(stderr, "Couldn't create SDL window or renderer: %s\n",
		        SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(mainRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
	SDL_SetRenderDrawColor(mainRenderer, P_BLACK);
	SDL_RenderClear(mainRenderer);
	SDL_RenderPresent(mainRenderer);
#ifndef WINDOWED
	SDL_ShowCursor(SDL_DISABLE);
#endif
	if (loadSounds()) {
		exit(EXIT_FAILURE);
	}
	if (initText(mainRenderer)) {
		exit(EXIT_FAILURE);
	}

	while (1) {
		if (titleScreen(mainRenderer)) {
			exit(EXIT_FAILURE);
		}

		if (doGame(mainRenderer, SDL_FALSE)) {
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

void cleanup(void)
{
	if (mainRenderer) {
		SDL_DestroyRenderer(mainRenderer);
	}
	if (mainWindow) {
		SDL_DestroyWindow(mainWindow);
	}
	SDL_Quit();
}
