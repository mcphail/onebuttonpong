#define _GNU_SOURCE 1
#include <stdio.h>
#include <SDL_mixer.h>
#include <SDL.h>

static Mix_Chunk *beep = NULL;
static Mix_Chunk *bing = NULL;
static Mix_Music *music = NULL;

static void cleanup(void);

int loadSounds(void)
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096)) {
		fprintf(stderr, "Failed to open audio mixer: %s\n",
		        Mix_GetError());
		return -1;
	}
	if (MIX_INIT_OGG != Mix_Init(MIX_INIT_OGG)) {
		fprintf(stderr, "Failed to load Ogg support: %s\n",
		        Mix_GetError());
		return -1;
	}

	atexit(cleanup);

	char *path;
	if (-1 == asprintf(&path, "%s/beep.ogg", PKGDATA)) {
		fprintf(stderr, "Failed to formulate sound path");
		return -1;
	}
	beep = Mix_LoadWAV(path);
	free(path);
	if (-1 == asprintf(&path, "%s/bing.ogg", PKGDATA)) {
		fprintf(stderr, "Failed to formulate sound path");
		return -1;
	}
	bing = Mix_LoadWAV(path);
	free(path);
	if (!beep || !bing) {
		fprintf(stderr, "Failed to load sample: %s\n",
		        Mix_GetError());
		return -1;
	}

	if (-1 == asprintf(&path, "%s/pipes.ogg", PKGDATA)) {
		fprintf(stderr, "Failed to formulate music path");
		return -1;
	}
	music = Mix_LoadMUS(path);
	free(path);
	if (!music) {
		printf("Failed to load music file: %s\n", Mix_GetError());
		return -1;
	}

	return 0;
}

int playBeep(void)
{
	if (Mix_PlayChannel(-1, beep, 0) == -1) {
		fprintf(stderr, "Failed to play beep: %s\n",
		        Mix_GetError());
		return -1;
	}
	return 0;
}

int playBing(void)
{
	if (Mix_PlayChannel(-1, bing, 0) == -1) {
		fprintf(stderr, "Failed to play bing: %s\n",
		        Mix_GetError());
		return -1;
	}
	return 0;
}

int playMusic(void)
{
	if (-1 == Mix_PlayMusic(music, -1)) {
		fprintf(stderr, "Failed to play music: %s\n",
		        Mix_GetError());
		return -1;
	}
	return 0;
}

void stopMusic(void)
{
	while(!Mix_FadeOutMusic(500) && Mix_PlayingMusic()) {
		SDL_Delay(50);
	}
}

void cleanup(void)
{
	SDL_Delay(200);
	Mix_FreeChunk(beep);
	Mix_FreeChunk(bing);
	Mix_FreeMusic(music);
	Mix_CloseAudio();
	while(Mix_Init(0)) {
		Mix_Quit();
	}
}
