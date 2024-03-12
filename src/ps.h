#ifndef PS_H_INCLUDED
#define PS_H_INCLUDED

#define ALLEGRO_STATICLINK

#include <allegro.h>
#include <stdlib.h>

#define MODE GFX_AUTODETECT_WINDOWED
#define WIDTH 800
#define HEIGHT 600

#define WHITE makecol(255,255,255)

#define MAX_BULLETS 1000
#define MAX_ENEMIES 200

BITMAP *buffer;
BITMAP *bg;

BITMAP *player_bmp;
BITMAP *bullet_bmp;
BITMAP *enemy_bmp;

// timer variables
volatile int counter;
volatile int ticks;
volatile int framerate;
volatile int resting, rested;

typedef struct sprite_t
{
    int x, y;
    int w, h;
    int speed;
    int alive;
    int xdelay,ydelay;
    int xcount,ycount;
    int id;
} Sprite;

Sprite *player;
Sprite *bullets[MAX_BULLETS];
Sprite *enemies[MAX_ENEMIES];

int gameover = 0;
int score = 0;

int bullet_counter = 0;
int bullet_delay = 20;
int bullet_count = 0;

int enemy_counter = 0;
int enemy_delay = 60;
int enemy_count = 0;

void initialize();
void load_assets();
void input();
void update();
void render();

void display_info();

void fire_bullet();
void update_sprite(Sprite *spr);
void update_bullet(Sprite *spr);
void update_player();
void update_bullets();
void update_enemies();

int is_inside(int x,int y,int left,int top,int right,int bottom);


#endif // PS_H_INCLUDED
