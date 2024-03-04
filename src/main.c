#define ALLEGRO_STATICLINK

#include <allegro.h>

#define MODE GFX_AUTODETECT_WINDOWED
#define WIDTH 800
#define HEIGHT 600
#define WHITE makecol(255,255,255)

// timer variables
volatile int counter;
volatile int ticks;
volatile int framerate;
volatile int resting, rested;

// calculate frame-rate every second
void timer1(void)
{
    counter++;
    framerate = ticks;
    ticks = 0;
    rested = resting;
}
END_OF_FUNCTION(timer1)

// do something while resting (?)
void rest1(void)
{
    resting++;
}
END_OF_FUNCTION(rest1)

void display_info(struct BITMAP *buffer)
{

    // display info
    textprintf_ex(buffer, font, 10, 10, WHITE, -1, "Ticks: %d", counter);

    textprintf_ex(buffer, font, 10, 20, WHITE, -1, "FPS: %d", framerate);
    textprintf_ex(buffer, font, 10, SCREEN_H - 20, WHITE, -1, "Press [ESC] to quit");
}

int main(void)
{
    int gameover = 0;

    BITMAP *buffer;
    BITMAP *bg;
    BITMAP *player;
    int x, y;

    // initialize
    allegro_init();
    set_color_depth(16);
    set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);
    install_keyboard();
    install_timer();
    srand(time(NULL));

    // lock interrupt variables
    LOCK_VARIABLE(counter);
    LOCK_VARIABLE(framerate);
    LOCK_VARIABLE(ticks);
    LOCK_VARIABLE(resting);
    LOCK_VARIABLE(rested);
    LOCK_FUNCTION(timer1);
    LOCK_FUNCTION(rest1);

    install_int(timer1, 1000);

    // create buffer
    buffer = create_bitmap(WIDTH,HEIGHT);

    // load background
    bg = load_bitmap("assets/bluespace.bmp", NULL);
    if (!bg)
    {
        allegro_message("background::\n%s", allegro_error);
        return 1;
    }

    // load player
    player = load_bitmap("assets/player.bmp", NULL);

    // start position of bitmap
    x = SCREEN_W/2 - player->w/2;
    y = SCREEN_H/2 - player->h/2;

    // game loop
    while (!gameover)
    {
        // INPUT
        //
        // short circuit
        if(key[KEY_ESC]) gameover = 1;

        // move bitmap
        if(key[KEY_LEFT]) x--;
        if(key[KEY_RIGHT]) x++;
        if(key[KEY_UP]) y--;
        if(key[KEY_DOWN]) y++;

        // UPDATE
        //
        // fill screen with background image
        blit(bg, buffer, 0, 0, 0, 0, WIDTH, HEIGHT);

        //draw the sprite
        draw_sprite(buffer, player, x, y);
        ticks++;

        // TODO: add toggle functionality to display info
        display_info(buffer);

        // DRAW
        //
        // refresh the screen
        acquire_screen();
        blit(buffer, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
        release_screen();

        // slow the game down
        resting = 0;
        rest_callback(13, rest1);

        // prevent above 60FPS
        if(framerate > 60) framerate = 60;
    }

    destroy_bitmap(bg);
    destroy_bitmap(buffer);
    destroy_bitmap(player);

    allegro_exit();
    return 0;
}
END_OF_MAIN()
