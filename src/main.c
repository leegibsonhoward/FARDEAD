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
    BITMAP *buffer;
    BITMAP *bg;
    BITMAP *player;
    int x, y;
    int xspeed = 2, yspeed = 2;
    int facing = 0;
    int gameover = 0;

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
        // INPUT /////////////////////////

        // short circuit
        if(key[KEY_ESC]) gameover = 1;

        // move bitmap
        if(key[KEY_LEFT])
        {
            facing = 0;
            x -= xspeed;
        }
        if(key[KEY_RIGHT])
        {
            facing = 1;
            x += xspeed;
        }
        if(key[KEY_UP])
        {
            y -= yspeed;
        }
        if(key[KEY_DOWN])
        {
            y += yspeed;
        }

        // UPDATE /////////////////////////////////

        // fill screen with background image
        blit(bg, buffer, 0, 0, 0, 0, WIDTH, HEIGHT);

        if(facing)
        {
            //draw the sprite
            draw_sprite(buffer, player, x, y);
        }
        else
        {
            draw_sprite_h_flip(buffer, player, x, y);
        }

        ticks++;


        // RENDER /////////////////////////////////////////

        // display allegro/system info
        // TODO: add toggle functionality to display info
        display_info(buffer);

        // lock bitmap
        acquire_screen();
        // refresh the screen
        blit(buffer, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
        // unlock bitmap
        release_screen();

        // slow down the game
        resting = 0;
        rest_callback(13, rest1);

        // prevent going above 60FPS
        // TODO: cap actual game speed
        if(framerate > 60) framerate = 60;
    }

    destroy_bitmap(bg);
    destroy_bitmap(buffer);
    destroy_bitmap(player);

    allegro_exit();
    return 0;
}
END_OF_MAIN()
