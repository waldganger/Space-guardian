#include "stage.h"


static void logic(void);
static void draw(void);
static void drawPlayer(void);
static void drawBullets(void);
static void initPlayer(void);
static void doPlayer(void);
static void doBullets(void);
static void fireBullet(void);
static int generateRandomNumber(unsigned int top);
static void doFighters(void);
static void spawnEnemies(void);
static void drawFighters(void);


static Entity* player;
static SDL_Texture* bulletTexture;
static SDL_Texture* enemyTexture;
static int enemySpawnTimer;


void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	memset(&stage, 0, sizeof(Stage));

	stage.fighterTail = &stage.fighterHead;
	stage.bulletTail = &stage.bulletHead;

	initPlayer();

	bulletTexture = loadTexture("gfx/playerBullet.png");
	enemyTexture = loadTexture("gfx/enemy.png");

	enemySpawnTimer = 0;
}

static void initPlayer(void)
{
	player = malloc(sizeof(Entity));
	if (player) memset(player, 0, sizeof(Entity));

	stage.fighterTail->next = player;
	stage.fighterTail = player;

	player->x = 100;
	player->y = 100;
	player->texture = loadTexture("gfx/player.png");

	SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
}

static void logic(void)
{
	doPlayer();
	doFighters();
	doBullets();
	spawnEnemies();
}

static void doPlayer(void)
{

	player->dx = 0;
	player->dy = 0;

	if (player->reload > 0) player->reload--;
	if (app.keyboard[SDL_SCANCODE_UP] && player->y) player->dy = -PLAYER_SPEED;
	if (app.keyboard[SDL_SCANCODE_DOWN] && player->y + player->h < SCREEN_HEIGHT) player->dy = PLAYER_SPEED;
	if (app.keyboard[SDL_SCANCODE_LEFT] && player->x > 0) player->dx = -PLAYER_SPEED;
	if (app.keyboard[SDL_SCANCODE_RIGHT] && player->x + player->w < SCREEN_WIDTH) player->dx = PLAYER_SPEED;
	if (app.keyboard[SDL_SCANCODE_LCTRL] && player->reload == 0) fireBullet();

	/*
	* This part is done in doFighters() now
	player->x += player->dx;
	player->y += player->dy;
	*/
}

static void fireBullet(void)
{
	Entity* bulletL;
	Entity* bulletR;
	float randomDY;

	bulletL = malloc(sizeof(Entity));
	bulletR = malloc(sizeof(Entity));
	if (bulletL) memset(bulletL, 0, sizeof(Entity));
	if (bulletR) memset(bulletR, 0, sizeof(Entity));
	randomDY = (float)generateRandomNumber(PLAYER_BULLET_SPEED);

	stage.bulletTail->next = bulletL;
	stage.bulletTail = bulletL;

	stage.bulletTail->next = bulletR;
	stage.bulletTail = bulletR;

	bulletL->x = player->x + player->w / 2;
	bulletL->y = player->y;
	bulletL->dx = PLAYER_BULLET_SPEED;
	bulletL->dy = randomDY;
	bulletL->health = 1;
	bulletL->texture = bulletTexture;
	SDL_QueryTexture(bulletL->texture, NULL, NULL, &bulletL->w, &bulletL->h);

	bulletR->x = player->x + player->w / 2;
	bulletR->y = player->y + player->h;
	bulletR->dx = PLAYER_BULLET_SPEED;
	bulletR->dy = randomDY;
	bulletR->health = 1;
	bulletR->texture = bulletTexture;
	bulletR->w = bulletL->w;
	bulletR->h = bulletL->h;

	/* 8 frames (approx 0.133333 seconds) must pass before we can fire again. */
	player->reload = 4;
}

static void doBullets()
{
	Entity* b;
	Entity* prev;

	prev = &stage.bulletHead;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		b->x += b->dx;
		b->y += b->dy;

		if (b->x > SCREEN_WIDTH)
		{
			if (b == stage.bulletTail) stage.bulletTail = prev;

			prev->next = b->next;
			free(b);
			b = prev;
		}
		prev = b;
	}
}

static void draw(void)
{
	drawPlayer();
	drawBullets();
	drawFighters();
}

static void drawPlayer(void)
{
	blit(player->texture, (int)player->x, (int)player->y);
}

static void drawBullets(void)
{
	Entity* b;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		blit(b->texture, (int)b->x, (int)b->y);
	}
}

static int generateRandomNumber(unsigned int top)
{
	/* Intializes random number generator */
	unsigned int seed;
	char randomNegativeSwitch;

	seed = SDL_GetTicks();
	randomNegativeSwitch = seed % 2 ? 1 : -1;

	return (rand() % top) * randomNegativeSwitch;
}

static void doFighters(void)
{
	Entity* e;
	Entity* prev;

	prev = &stage.fighterHead;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		e->x += e->dx;
		if (e->y >= (SCREEN_HEIGHT - e->h) || e->y < 0) e->dy = -(e->dy);
		e->y += e->dy;

		if (e != player && e->x < -e->w)
		{
			if (e == stage.fighterTail) stage.fighterTail = prev;
			prev->next = e->next;
			free(e);
			e = prev;
		}
		prev = e;
	}
}

void spawnEnemies(void)
{
	Entity* enemy;
	char randomDy;

	if (--enemySpawnTimer <= 0)
	{
		enemy = malloc(sizeof(Entity));
		if(enemy) memset(enemy, 0, sizeof(Entity));
		stage.fighterTail->next = enemy;
		stage.fighterTail = enemy;

		enemy->texture = enemyTexture;
		SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

		enemy->x = SCREEN_WIDTH;
		enemy->y = 5 + (rand() % SCREEN_HEIGHT - enemy->h); /* TODO : bug car vaisseau peut apparaître trop haut*/
		enemy->dx = -(2 + (rand() % 4));
		randomDy = rand() % 2;
		enemy->dy = randomDy ? 1 : -1;;

		

		enemySpawnTimer = 30 + (rand() % 60); /* creates an enemy every 30 <-> 90 ms */
	}
}

static void drawFighters(void)
{
	Entity* e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		blit(e->texture, e->x, e->y);
	}
}