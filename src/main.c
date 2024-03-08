#define ALLEGRO_STATICLINK

#include <allegro.h>

#define MODE GFX_AUTODETECT_WINDOWED
#define WIDTH 800
#define HEIGHT 600
#define WHITE makecol(255,255,255)

BITMAP *bullet_bmp;
BITMAP *buffer;
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

void display_info(struct BITMAP *buffer, int x, int y) // x and y track player position.
{
    // display info
    textprintf_ex(buffer, font, 10, 10, WHITE, -1, "Ticks: %d", counter);
    textprintf_ex(buffer, font, 10, 20, WHITE, -1, "FPS: %d", framerate);
    textprintf_ex(buffer, font, 10, 30, WHITE, -1, "Player Position: x %d, y %d", x, y);
    textprintf_ex(buffer, font, 10, SCREEN_H - 20, WHITE, -1, "Press [ESC] to quit");
}

struct playerTag
{
    int x;
    int y;
    int w;
    int h;
    int speed;
    int facing;
} player;

struct bulletTag
{
    int x;
    int y;
    int alive;
    int speed;
} bullet;

void firebullet()
{
    // TEST BULLET
    bullet.alive = 1;
    bullet.x = player.x + 44;
    bullet.y = player.y + 24;
    //load bullet image if necessary
    if (bullet_bmp == NULL)
    {
        bullet_bmp = load_bitmap("assets/bullet.bmp", NULL);
    }
}

int main(void)
{
    BITMAP *bg;
    BITMAP *player_bmp;
    int gameover = 0;

    // initialize
    allegro_init();
    set_color_depth(24);
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
    player_bmp = load_bitmap("assets/player.bmp", NULL);// load player

    // start position and speed of player
    player.x = SCREEN_W/2 - player.w/2;
    player.y = SCREEN_H/2 - player.h/2;
    player.speed = 2;
    bullet.speed = 4;

    // game loop
    while (!gameover)
    {
        // fill screen with background image
        blit(bg, buffer, 0, 0, 0, 0, WIDTH, HEIGHT);
        // INPUT /////////////////////////

        // short circuit
        if(key[KEY_ESC]) gameover = 1;

        // move bitmap
        if(key[KEY_LEFT] || key[KEY_A])
        {
            player.facing = 0;
            player.x -= player.speed;
        }
        if(key[KEY_RIGHT] || key[KEY_D])
        {
            player.facing = 1;
            player.x += player.speed;
        }
        if(key[KEY_UP] || key[KEY_W])
        {
            player.y -= player.speed;
        }
        if(key[KEY_DOWN] || key[KEY_S])
        {
            player.y += player.speed;
        }
        if (key[KEY_SPACE])
        {
            firebullet();
        }

        // UPDATE /////////////////////////////////

        if(bullet.alive)
        {
            // this is so much like the gouls from super mario when you
            // dont look at them they come for you without retreating.
            if(player.facing)
            {
                bullet.x += (bullet.speed);
            } else
                bullet.x -= (bullet.speed);

            draw_sprite(buffer, bullet_bmp, bullet.x, bullet.y);
        }
        if(player.facing)
        {
            //draw the sprite
            draw_sprite(buffer, player_bmp, player.x, player.y);
        }
        else
        {
            draw_sprite_h_flip(buffer, player_bmp, player.x, player.y);
        }

        ticks++;

        // RENDER /////////////////////////////////////////

        // display allegro/system info
        // TODO: add toggle functionality to display info
        display_info(buffer, player.x, player.y);

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
    destroy_bitmap(player_bmp);

    allegro_exit();
    return 0;
}
END_OF_MAIN()
