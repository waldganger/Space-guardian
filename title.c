#include "title.h"

static void logic(void);
static void draw(void);
static void drawTitle(void);

static SDL_Texture* titleTexture;

static int revealH = 200;
static int revealW = 200;
static int timeout;

void initTitle(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

	titleTexture = loadTexture("gfx/title.png");
	
	timeout = FPS * 15;
}

static void logic(void)
{
	doBackground();
	doStarfield();

	if (revealH < SCREEN_HEIGHT)
	{
		revealH++;
	}

	if (revealW < SCREEN_WIDTH)
	{
		revealW += 4;
	}

	if (--timeout <= 0)
	{
		initHighscores();
	}

	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		initStage();
	}
}

static void draw(void)
{
	drawBackground();
	drawStarfield();
	drawTitle();

	if (timeout % 40 < 20)						// blink text
	{
		drawText(SCREEN_WIDTH / 2, 600, 255, 255, 255, 1, TEXT_CENTER, "PRESS SPACE TO PLAY!");
	}
}

static void drawTitle(void)
{
	SDL_Rect r;

	r.x = 0;
	r.y = 0;

	SDL_QueryTexture(titleTexture, NULL, NULL, &r.w, &r.h);

	r.w = MIN(revealW, r.w);
	r.h = MIN(revealH, r.h);

	blitRect(titleTexture, &r, (SCREEN_WIDTH / 2) - (r.w / 2), 100);
}