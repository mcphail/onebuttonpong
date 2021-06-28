#define _GNU_SOURCE 1
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "colours.h"
#include "game.h"

static TTF_Font *font;
static SDL_Texture *glyphs[10];

static void cleanup(void);

int initText(SDL_Renderer *renderer)
{
	atexit(cleanup);
	if (TTF_Init()) {
		fprintf(stderr, "Failed to initialize fonts: %s\n",
		        TTF_GetError());
		return -1;
	}

	char *path;
	if (-1 == asprintf(&path, "%s/zxsp____.ttf", PKGDATA)) {
		fprintf(stderr, "Failed to forumale font path\n");
		return -1;
	}
	font = TTF_OpenFont(path, 100);
	free(path);
	if (!font) {
		fprintf(stderr, "Failed to load font: %s\n",
		        TTF_GetError());
		return -1;
	}

	SDL_Color color = {P_CYAN};
	for (int ch = 48; ch < 58; ch++) {
		SDL_Surface *surfaceGlyph;
		surfaceGlyph = TTF_RenderGlyph_Blended(font, ch, color);
		if (!surfaceGlyph) {
			fprintf(stderr, "Couldn't load glyph %d\n", ch - 48);
			return -1;
		}
		glyphs[ch - 48] = SDL_CreateTextureFromSurface(renderer,
		                  surfaceGlyph);
		SDL_FreeSurface(surfaceGlyph);
		if (! glyphs[ch - 48]) {
			fprintf(stderr, "Failed to make texture %d: %s\n",
			        ch - 48, SDL_GetError());
			return -1;
		}
	}

	return 0;
}

int glyphWidth(int number)
{
	int width = 0;
	if (SDL_QueryTexture(glyphs[number], NULL, NULL, &width, NULL)) {
		fprintf(stderr, "Failed to query texture: %s\n",
		        SDL_GetError());
		return 0;
	}
	return width;
}

int glyphHeight(int number)
{
	int height = 0;
	if (SDL_QueryTexture(glyphs[number], NULL, NULL, NULL, &height)) {
		fprintf(stderr, "Failed to query texture: %s\n",
		        SDL_GetError());
		return 0;
	}
	return height;
}

int printScore(int p1, int p2, SDL_Renderer *renderer)
{
	SDL_Rect p1r = {0, 0, glyphWidth(p1), glyphHeight(p1)};
	SDL_Rect p2r = {0, 0, glyphWidth(p2), glyphHeight(p2)};
	int freeWidth = SCREEN_WIDTH - p1r.w - p2r.w;
	p1r.x = freeWidth / 3;
	p2r.x = 2 * freeWidth / 3 + p1r.w;
	p1r.y = SCREEN_HEIGHT / 5;
	p2r.y = p1r.y;

	if ((SDL_RenderCopy(renderer, glyphs[p1], NULL, &p1r)) +
	        (SDL_RenderCopy(renderer, glyphs[p2], NULL, &p2r))) {
		fprintf(stderr, "Score render failure: %s\n",
		        SDL_GetError());
		return -1;
	}

	return 0;
}

void cleanup(void)
{
	if (font) {
		TTF_CloseFont(font);
	}

	if (glyphs[0]) {
		for (int i = 0; i < 10; i++) {
			SDL_DestroyTexture(glyphs[i]);
		}
	}
	TTF_Quit();
}
