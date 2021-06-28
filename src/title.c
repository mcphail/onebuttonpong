#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "sounds.h"

#include "../res/pixmaps/title.xpm"

int titleScreen(SDL_Renderer *renderer)
{
	SDL_Surface *screenSurface = IMG_ReadXPMFromArray(title_xpm);
	if (! screenSurface) {
		fprintf(stderr, "Error loading title screen: %s\n",
		        IMG_GetError());
		return -1;
	}

	SDL_RenderClear(renderer);
	SDL_Texture *screenTexture = SDL_CreateTextureFromSurface(renderer,
	                             screenSurface);
	if (! screenTexture) {
		fprintf(stderr, "Error loading title screen to texture: %s\n",
		        SDL_GetError());
		SDL_FreeSurface(screenSurface);
		return -1;
	}

	SDL_FreeSurface(screenSurface);

	if (SDL_RenderCopy(renderer, screenTexture, NULL, NULL)) {
		fprintf(stderr, "Error rendering title screen: %s\n",
		        SDL_GetError());
		SDL_DestroyTexture(screenTexture);
		return -1;
	}

	SDL_RenderPresent(renderer);
	
	SDL_Event key_press;
	int keep_looping=1;
	int quit_or_play;
	playMusic();
	SDL_FlushEvent(SDL_KEYDOWN);
	SDL_Delay(500);
	SDL_FlushEvent(SDL_KEYDOWN);
	while (keep_looping) {
		if (SDL_WaitEvent(&key_press)) {
			if (key_press.type == SDL_KEYDOWN) {
				if (key_press.key.keysym.scancode ==
					SDL_SCANCODE_ESCAPE) {
					keep_looping=0;
					quit_or_play=1;
				}
				if (key_press.key.keysym.scancode ==
					SDL_SCANCODE_SPACE) {
					keep_looping=0;
					quit_or_play=0;
				}
			}
		}
	}
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(screenTexture);
	stopMusic();
	return quit_or_play;
}
