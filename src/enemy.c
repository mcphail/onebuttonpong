#include <SDL.h>
#include <stdlib.h>

#include "enemy.h"
#include "game.h"

int moveEnemy(const SDL_Rect *bat, const SDL_Rect *ball, float ballRad)
{
	while (ballRad >= (2 * M_PI)) {
		ballRad -= 2 * M_PI;
	}
	while (ballRad < 0) {
		ballRad += 2 * M_PI;
	}
	/* If ball is going left, return paddle to middle of court */
	if (ballRad > M_PI) {
		if (bat->y > (SCREEN_HEIGHT / 2 - bat->h / 2)) {
			return -1;
		} else if (bat->y < (SCREEN_HEIGHT / 2 - bat->h / 2)) {
			return 1;
		} else {
			return 1;
		}
	}

	/* Move randomly when ball is far, to reduce difficulty */
	if (ball->x < bat->x * 0.6) {
		if (rand() > RAND_MAX / 2) {
			return 1;
		}
		else {
			return -1;
		}
	}

	int x = ball->x;
	int y = ball->y;
	while (x <= bat->x) {
		double dx = 10 * sin(ballRad);
		double dy = 10 * cos(ballRad);
		x += dx;
		y -= dy;
		if (y < BOARD_Y) {
			y = 2 * BOARD_Y - y;
			ballRad = M_PI - ballRad;
		}
		if (y > (BOARD_Y + BOARD_HEIGHT - ball->h)) {
			y = y - 2 * (y - BOARD_Y - BOARD_HEIGHT - ball->h);
			ballRad = M_PI - ballRad;
		}
	}
	if ((bat->y + bat->h / 2) > y) {
		return -1;
	} else if ((bat->y + bat->h / 2) < y) {
		return 1;
	} else {
		return 1;
	}
}
