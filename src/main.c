
#include "ps.h"

int main(void)
{
    // setup allegro
    initialize();

    load_assets();

    // game loop
    while (!gameover)
    {
        input();

        update();

        render();
    }

    free(player);
    // free resources
    for(int i =0; i < MAX_BULLETS; i++)
    {
        free(bullets[i]);
    }

    for(int i =0; i < MAX_ENEMIES; i++)
    {
        free(enemies[i]);
    }

    destroy_bitmap(bg);
    destroy_bitmap(buffer);
    destroy_bitmap(player_bmp);
    destroy_bitmap(bullet_bmp);

    allegro_exit();
    return 0;
}
END_OF_MAIN()


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

void initialize()
{
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

    // start interrupt timer
    install_int(timer1, 1000);

    // create buffer
    buffer = create_bitmap(WIDTH,HEIGHT);
}

void load_assets()
{
    // load background
    bg = load_bitmap("assets/bluespace.bmp", NULL);
    if (!bg)
    {
        allegro_message("background::\n%s", allegro_error);
        return;
    }

    // load player
    player_bmp = load_bitmap("assets/player.bmp", NULL);// load player

    if (bullet_bmp == NULL)
    {
        bullet_bmp = load_bitmap("assets/bullet.bmp", NULL);
    }

    if (enemy_bmp == NULL)
    {
        enemy_bmp = load_bitmap("assets/enemy.bmp", NULL);
    }

    player = (Sprite*)malloc(sizeof(Sprite));
    player->x = 150;
    player->y = 200;
    player->speed = 2;


    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i] = (Sprite*)malloc(sizeof(Sprite));
        bullets[i]->x = 0;
        bullets[i]->y = 0;
        bullets[i]->speed = 0;
        bullets[i]->alive = 0;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = (Sprite*)malloc(sizeof(Sprite));
        enemies[i]->x = SCREEN_W;
        enemies[i]->y = 200;
        enemies[i]->speed = 0;
        enemies[i]->alive = 0;
    }
}

void input()
{
    // INPUT /////////////////////////

    // short circuit
    if(key[KEY_ESC]) gameover = 1;

    // move bitmap
    if(key[KEY_LEFT] || key[KEY_A])
    {
        player->x -= player->speed;
    }
    if(key[KEY_RIGHT] || key[KEY_D])
    {
        player->x += player->speed;
    }
    if(key[KEY_UP] || key[KEY_W])
    {
        player->y -= player->speed;
    }
    if(key[KEY_DOWN] || key[KEY_S])
    {
        player->y += player->speed;
    }
    if (key[KEY_SPACE])
    {
        if (bullet_counter > bullet_delay)
        {
            bullet_counter = 0;
            fire_bullet();
        }
    }

}

void update()
{
    // UPDATE /////////////////////////////////

    ticks++;
    bullet_counter++;
    enemy_counter++;

    // slow down the game
    resting = 0;
    rest_callback(13, rest1);
}

void render()
{
    // RENDER /////////////////////////////////////////

    // draw background
    blit(bg, buffer, 0, 0, 0, 0, WIDTH, HEIGHT);

    //draw player
    draw_sprite(buffer, player_bmp, player->x, player->y);

    // draw bullets
    for(int i=0; i < MAX_BULLETS; i++)
    {
        if(bullets[i]->alive)
        {
            bullets[i]->speed = 4;
            bullets[i]->x += (bullets[i]->speed);
            draw_sprite(buffer, bullet_bmp,  bullets[i]->x, bullets[i]->y);
        }
    }

    // draw enemies
    for(int i = 0; i  < MAX_ENEMIES; i++)
    {
        if(enemies[i]->alive)
        {
            enemies[i]->speed = 1;
            enemies[i]->x -= (enemies[i]->speed);

            draw_sprite_h_flip(buffer, enemy_bmp, enemies[i]->x, enemies[i]->y);
        }
    }

    // spawn enemies with delay
    if(enemy_counter > enemy_delay)
    {
        enemy_counter = 0;
        spawn_enemy();
    }

    // display allegro/system info
    // TODO: add toggle functionality to display info
    display_info(buffer, player->x, player->y);

    // lock bitmap
    acquire_screen();
    // refresh the screen
    blit(buffer, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
    // unlock bitmap
    release_screen();

    // prevent going above 60FPS
    // TODO: cap actual game speed
    if(framerate > 60) framerate = 60;

}

void display_info(struct BITMAP *buffer, int x, int y) // x and y track player position.
{
    // display info
    textprintf_ex(buffer, font, 10, 10, WHITE, -1, "Ticks: %d", counter);
    textprintf_ex(buffer, font, 10, 20, WHITE, -1, "FPS: %d", framerate);
    textprintf_ex(buffer, font, 10, 30, WHITE, -1, "Player Position: x %d, y %d", x, y);
    textprintf_ex(buffer, font, 10, 40, WHITE, -1, "Enemy Count: %d", enemy_count);
    textprintf_ex(buffer, font, 10, 50, WHITE, -1, "Bullet Count: %d", bullet_count);
    textprintf_ex(buffer, font, 10, SCREEN_H - 20, WHITE, -1, "Press [ESC] to quit");
}

void fire_bullet()
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(!bullets[i]->alive)
        {
            bullet_count++;
            bullets[i]->alive++;
            bullets[i]->x = player->x + 44; // arbitrary location: tip
            bullets[i]->y = player->y + 24; // of players gun.
            return;
        }
    }
}
void spawn_enemy()
{
    for(int i = 0; i < MAX_ENEMIES; i++)
    {
        if(!enemies[i]->alive)
        {
            enemy_count++;
            enemies[i]->alive++;
            enemies[i]->x = enemies[i]->x;
            enemies[i]->y = rand() % SCREEN_H - 50; // randomize vertical position.
            return;
        }
    }
}
