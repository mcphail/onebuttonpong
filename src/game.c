#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "colours.h"
#include "sounds.h"
#include "text.h"
#include "enemy.h"

#define BORDER_X (BOARD_X - 1)
#define BORDER_Y (BOARD_Y - 1)
#define BORDER_WIDTH (BOARD_WIDTH + 2)
#define BORDER_HEIGHT (BOARD_HEIGHT + 2)
#define BAT_SPEED 5
#define POLL_INTERVAL 20
#define BALL_SIZE 10

static int drawBoard(SDL_Renderer *);
static int drawBat(SDL_Renderer *, SDL_Rect *);
static int drawBall(SDL_Renderer *, SDL_Rect *);
static double wallBounce(SDL_Rect *, double);
static void cleanupBoard(void);

static SDL_Texture *boardTexture = NULL;

int doGame(SDL_Renderer *renderer, SDL_bool twoplayer)
{
	SDL_assert(SCREEN_WIDTH >= BOARD_WIDTH);
	SDL_assert(SCREEN_HEIGHT >= BOARD_HEIGHT);
	SDL_Color savedColours;
	if (SDL_GetRenderDrawColor(renderer,
	                           &savedColours.r,
	                           &savedColours.g,
	                           &savedColours.b,
	                           &savedColours.a)) {
		fprintf(stderr, "Failed to get renderer draw colour: %s\n",
		        SDL_GetError());
		return -1;
	}
	if (SDL_SetRenderDrawColor(renderer, P_BLACK) +
	        SDL_RenderClear(renderer)) {
		fprintf(stderr, "Failed to clear screen: %s\n", SDL_GetError());
		return -1;
	}
	if (drawBoard(renderer)) {
		return -1;
	}
	SDL_RenderPresent(renderer);

	/* Main loop starts here */
	srand(SDL_GetTicks());
	SDL_bool looping = SDL_TRUE;
	Uint8 score1 = 0;
	Uint8 score2 = 0;
	Uint32 oldTime, startTime, newTime = 0;
	double ballSpeed = BAT_SPEED;
	double ballRad = (double)rand() / (double)RAND_MAX * 2.0 * M_PI;
	/* Start ball off in sensible directions only */
	if ((ballRad < (0.25 * M_PI)) || ((ballRad > (0.75 * M_PI)) &&
	                                  (ballRad < (1.25 * M_PI))) || (ballRad > (1.75 * M_PI))) {
		ballRad += (M_PI / 2);
	}
	SDL_Rect bat1[3] = {{
			BOARD_X + BOARD_WIDTH / 8,
			BOARD_Y + 3 * BOARD_HEIGHT / 7,
			5,
			BOARD_HEIGHT / 7
		},bat1[0],bat1[0]
	};
	SDL_Rect bat2[3] = {{
			BOARD_X + 7 * BOARD_WIDTH / 8 - 5,
			bat1[0].y,
			5, bat1[0].h
		},bat2[0],bat2[0]
	};
	SDL_Rect ball[3] = {{
			BOARD_X + BOARD_WIDTH / 2 - 5,
			BOARD_Y + BOARD_HEIGHT / 2 - 5,
			BALL_SIZE, BALL_SIZE
		}, ball[0], ball[0]
	};
	for (oldTime = SDL_GetTicks(), startTime = oldTime;
	        (score1 < 9) && (score2 < 9) && looping;
	        SDL_Delay(5), newTime = SDL_GetTicks()) {

		if (newTime > (oldTime + POLL_INTERVAL)) {
			double batMove = BAT_SPEED * (newTime - oldTime) /
			                 POLL_INTERVAL;
			if (SDL_RenderClear(renderer)) {
				fprintf(stderr, "Couldn't clear screen: %s\n",
				        SDL_GetError());
				return -1;
			}
			SDL_PumpEvents();
			/* note: *state should not be freed */
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_ESCAPE]) {
				looping = SDL_FALSE;
			}
			int fall = 1;
			if (state[SDL_SCANCODE_SPACE]) {
				fall = 0;
				bat1[0].y -= batMove;
				if (bat1[0].y < BOARD_Y) {
					bat1[0].y = BOARD_Y;
				}
			}
			if (fall) {
				bat1[0].y += batMove;
				if (bat1[0].y > (BOARD_Y + BOARD_HEIGHT -
				                 bat1[0].h)) {
					bat1[0].y = BOARD_Y + BOARD_HEIGHT -
					            bat1[0].h;
				}
			}
			int em = 0;
			if (! twoplayer) {
				em = moveEnemy(&bat2[0], &ball[0], ballRad);
			}
			if ((state[SDL_SCANCODE_P] && twoplayer) || (-1 == em)) {
				bat2[0].y -= batMove;
				if (bat2[0].y < BOARD_Y) {
					bat2[0].y = BOARD_Y;
				}
			}
			if ((state[SDL_SCANCODE_L] && twoplayer) || (1 == em)) {
				bat2[0].y += batMove;
				if (bat2[0].y > (BOARD_Y + BOARD_HEIGHT -
				                 bat2[0].h)) {
					bat2[0].y = BOARD_Y + BOARD_HEIGHT -
					            bat2[0].h;
				}
			}

			double dx, dy;
			dx = ballSpeed * sin(ballRad);
			dy = ballSpeed * cos(ballRad);
			dx = dx * (newTime - oldTime) / POLL_INTERVAL;
			dy = dy * (newTime - oldTime) / POLL_INTERVAL;
			ball[0].x += dx;
			ball[0].y -= dy;
			SDL_Rect collision;
			SDL_UnionRect(&ball[0], &ball[1], &collision);
			ballRad = wallBounce(&ball[0], ballRad);
			if (SDL_HasIntersection(&collision, &bat1[0])) {
				playBeep();
				ball[0].x = 2 * (bat1[0].x + bat1[0].w) -
				            ball[0].x;
				ballRad = 2 * M_PI - ballRad;
				if (bat1[0].y < bat1[1].y) {
					ballRad *= 0.8;
					if (ballRad < (0.15 * M_PI)) {
						ballRad = 0.15 * M_PI;
					}
				}
				if (bat1[0].y > bat1[1].y) {
					ballRad *= 1.2;
					if (ballRad > (0.85 * M_PI)) {
						ballRad = 0.85 * M_PI;
					}
				}
			}
			if (SDL_HasIntersection(&collision, &bat2[0])) {
				playBeep();
				ball[0].x = 2 * bat2[0].x - ball[0].x -
				            2 * BALL_SIZE;
				ballRad = 2 * M_PI - ballRad;
				if (bat2[0].y < bat2[1].y) {
					ballRad *= 1.2;
					if (ballRad > (1.85 * M_PI)) {
						ballRad = 1.85 * M_PI;
					}
				}
				if (bat2[0].y > bat2[1].y) {
					ballRad *= 0.8;
					if (ballRad < (1.15 * M_PI)) {
						ballRad = 1.15 * M_PI;
					}
				}
			}
			ballRad = wallBounce(&ball[0], ballRad);

			if ((ball[0].x + BALL_SIZE) < BOARD_X) {
				score2++;
				playBing();
				SDL_Delay(500);
				ballSpeed = BAT_SPEED;
				ballRad = (double)rand() / (double)RAND_MAX * 2.0 * M_PI;
				if ((ballRad < (0.25 * M_PI)) || ((ballRad > (0.75 * M_PI)) &&
				                                  (ballRad < (1.25 * M_PI))) || (ballRad > (1.75 * M_PI))) {
					ballRad += (M_PI / 2);
				}
				ball[0].x = BOARD_X + BOARD_WIDTH / 2 - 5;
				ball[0].y = BOARD_Y + BOARD_HEIGHT / 2 - 5;
				ball[1] = ball[0];
				ball[2] = ball[0];
				newTime = SDL_GetTicks();
				startTime = newTime;
			}
			if (ball[0].x > (BOARD_X + BOARD_WIDTH)) {
				score1++;
				playBing();
				SDL_Delay(500);
				ballSpeed = BAT_SPEED;
				ballRad = (double)rand() / (double)RAND_MAX * 2.0 * M_PI;
				if ((ballRad < (0.25 * M_PI)) || ((ballRad > (0.75 * M_PI)) &&
				                                  (ballRad < (1.25 * M_PI))) || (ballRad > (1.75 * M_PI))) {
					ballRad += (M_PI / 2);
				}
				ball[0].x = BOARD_X + BOARD_WIDTH / 2 - 5;
				ball[0].y = BOARD_Y + BOARD_HEIGHT / 2 - 5;
				ball[1] = ball[0];
				ball[2] = ball[0];
				newTime = SDL_GetTicks();
				startTime = newTime;
			}


			if ((drawBoard(renderer)) +
			        (printScore(score1, score2, renderer)) +
			        (drawBat(renderer, bat1)) +
			        (drawBat(renderer, bat2)) +
			        (drawBall(renderer, ball))
			   ) {
				return -1;
			}
			SDL_RenderPresent(renderer);
			oldTime = newTime;
			ballSpeed = BAT_SPEED + BAT_SPEED * (newTime - startTime) / 10000;
		}
	}

	if (SDL_SetRenderDrawColor(renderer,
	                           savedColours.r,
	                           savedColours.g,
	                           savedColours.b,
	                           savedColours.a)) {
		fprintf(stderr, "Failed to reset renderer draw colour: %s\n",
		        SDL_GetError());
		return -1;
	}
	SDL_RenderClear(renderer);
	printScore(score1, score2, renderer);
	SDL_RenderPresent(renderer);
	SDL_Delay(1000);
	return 0;
}

static int drawBoard(SDL_Renderer *renderer)
{
	atexit(cleanupBoard);
	if (boardTexture) {
		if(SDL_RenderCopy(renderer, boardTexture, NULL, NULL)) {
			fprintf(stderr, "Failed to blit board texture: %s\n",
			        SDL_GetError());
			return -1;
		} else {
			return 0;
		}
	}

	Uint32 *pixels;
	pixels = SDL_malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
	if (!pixels) {
		fprintf(stderr, "Failed to allocate memory!\n");
		return -1;
	}
	for (int i = 0; i < (SCREEN_HEIGHT * SCREEN_WIDTH); i++) {
		pixels[i] = 0xff000000;
	}
	for (int i = SCREEN_WIDTH * BORDER_Y + BORDER_X;
	        i < SCREEN_WIDTH * BORDER_Y + BORDER_X + BORDER_WIDTH +1; i++) {
		pixels[i] = 0xffffff00;
		pixels[i + SCREEN_WIDTH * BOARD_HEIGHT] = 0xffffff00;
	}
	int centreWidth = 2 - BOARD_WIDTH % 2;
	for (int i = 0; i < BOARD_HEIGHT; i ++) {
		pixels[SCREEN_WIDTH * (BOARD_Y + i) + BORDER_X]
		= 0xffffff00;
		pixels[SCREEN_WIDTH * (BOARD_Y + i) + BORDER_X + BORDER_WIDTH]
		= 0xffffff00;
		if (! (i % 2)) {
			pixels[SCREEN_WIDTH * (BOARD_Y + i) + SCREEN_WIDTH / 2]
			= 0xffffff00;
			if (centreWidth - 1) {
				pixels[SCREEN_WIDTH * (BOARD_Y + i)
				       + SCREEN_WIDTH / 2 + 1] = 0xffffff00;
			}
		}
	}
	boardTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                                 SDL_TEXTUREACCESS_STATIC,
	                                 SCREEN_WIDTH,
	                                 SCREEN_HEIGHT);
	if (!boardTexture) {
		fprintf(stderr, "Failed to create board texture: %s\n",
		        SDL_GetError());
		SDL_free(pixels);
		return -1;
	}
	if (SDL_UpdateTexture(boardTexture, NULL, pixels,
	                      SCREEN_WIDTH * sizeof(Uint32))) {
		fprintf(stderr, "Failed to make texture from pixels: %s\n",
		        SDL_GetError());
		SDL_free(pixels);
		return -1;
	}
	SDL_free(pixels);
	return drawBoard(renderer);
}


static int drawBat(SDL_Renderer *renderer, SDL_Rect *bat)
{
	SDL_Color savedColours;
	if (SDL_GetRenderDrawColor(renderer,
	                           &savedColours.r,
	                           &savedColours.g,
	                           &savedColours.b,
	                           &savedColours.a)) {
		fprintf(stderr, "Failed to get renderer draw colour: %s\n",
		        SDL_GetError());
		return -1;
	}
//#ifdef GHOSTING
	if (SDL_SetRenderDrawColor(renderer, P_WHITE_50)) {
		fprintf(stderr, "Failed to set render colour: %s\n",
		        SDL_GetError());
		return -1;
	}

	if (SDL_RenderFillRect(renderer, &bat[2])) {
		fprintf(stderr, "Failed to blit: %s\n", SDL_GetError());
		return -1;
	}

	if (SDL_SetRenderDrawColor(renderer, P_WHITE_75)) {
		fprintf(stderr, "Failed to set render colour: %s\n",
		        SDL_GetError());
		return -1;
	}

	if (SDL_RenderFillRect(renderer, &bat[1])) {
		fprintf(stderr, "Failed to blit: %s\n", SDL_GetError());
		return -1;
	}
//#endif
	if (SDL_SetRenderDrawColor(renderer, P_WHITE)) {
		fprintf(stderr, "Failed to set render colour: %s\n",
		        SDL_GetError());
		return -1;
	}

	if (SDL_RenderFillRect(renderer, &bat[0])) {
		fprintf(stderr, "Failed to blit: %s\n", SDL_GetError());
		return -1;
	}

	bat[2] = bat[1];
	bat[1] = bat[0];

	if (SDL_SetRenderDrawColor(renderer,
	                           savedColours.r,
	                           savedColours.g,
	                           savedColours.b,
	                           savedColours.a)) {
		fprintf(stderr, "Failed to reset renderer draw colour: %s\n",
		        SDL_GetError());
		return -1;
	}
	return 0;
}

static int drawBall(SDL_Renderer *renderer, SDL_Rect *ball)
{
	return drawBat(renderer, ball);
}

static double wallBounce(SDL_Rect *ball, double ballRad)
{
	if (ball->y < BOARD_Y) {
		ball->y = 2 * BOARD_Y - ball->y;
		ballRad = M_PI - ballRad;
	}
	if (ball->y > (BOARD_Y + BOARD_HEIGHT - BALL_SIZE)) {
		ball->y = ball->y - 2 * (ball->y -
		                         BOARD_Y - BOARD_HEIGHT + BALL_SIZE);
		ballRad = M_PI - ballRad;
	}
	while (ballRad > (2 * M_PI)) {
		ballRad -= 2 * M_PI;
	}
	while (ballRad < 0) {
		ballRad += 2 * M_PI;
	}
	return ballRad;
}

void cleanupBoard(void)
{
	if (boardTexture) {
		SDL_DestroyTexture(boardTexture);
	}
}
