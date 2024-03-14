
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

    // free all resources

    free(player);

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
    player->w = 50;
    player->h = 50;
    player->speed = 3;
    player->alive = 0;
    player->xdelay = 0;
    player->ydelay = 0;
    player->xcount = 0;
    player->ycount = 0;
    player->id = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = (Sprite*)malloc(sizeof(Sprite));
        enemies[i]->x = SCREEN_W;
        enemies[i]->y = rand() % ((SCREEN_H - 50) + 1);
        enemies[i]->w = 50;
        enemies[i]->h = 50;
        enemies[i]->speed = 2;
        enemies[i]->alive = 0;
        enemies[i]->dead = 0;
        enemies[i]->xdelay = 0;
        enemies[i]->ydelay = 0;
        enemies[i]->xcount = 0;
        enemies[i]->ycount = 0;
        enemies[i]->id = 1;
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i] = (Sprite*)malloc(sizeof(Sprite));
        bullets[i]->x = 0;
        bullets[i]->y = 0;
        bullets[i]->w = 9;
        bullets[i]->h = 9;
        bullets[i]->speed = 5;
        bullets[i]->alive = 0;
        bullets[i]->dead = 0;
        bullets[i]->xdelay = 0;
        bullets[i]->ydelay = 0;
        bullets[i]->xcount = 0;
        bullets[i]->ycount = 0;
        bullets[i]->id = 2;
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
        if (player->x < 0)
            player->x = 0;
    }
    if(key[KEY_RIGHT] || key[KEY_D])
    {
        player->x += player->speed;
        if (player->x > (SCREEN_W - player->w) / 4)
            // player right bounds 1/4 of screen width
            player->x = (SCREEN_W - player->w) / 4;
    }
    if(key[KEY_UP] || key[KEY_W])
    {
        player->y -= player->speed;
        if (player->y < 0)
            player->y = 0;
    }
    if(key[KEY_DOWN] || key[KEY_S])
    {
        player->y += player->speed;
        if (player->y > SCREEN_H - player->h)
            player->y = SCREEN_H - player->h;
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

    update_player();

    update_enemies();

    update_bullets();

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
    textprintf_ex(buffer, font, 10, 40, WHITE, -1, "Enemies Spawned: %d", enemy_count);
    textprintf_ex(buffer, font, 10, 50, WHITE, -1, "Enemies Killed: %d", enemies_killed);
    textprintf_ex(buffer, font, 10, 60, WHITE, -1, "Bullet Count: %d", bullet_count);
    textprintf_ex(buffer, font, 10, 70, WHITE, -1, "Score: %d", score);
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

void update_sprite(Sprite *spr)
{
    //update x position
    if (++spr->xcount > spr->xdelay)
    {
        spr->xcount = 0;

        if(spr->id == 0)
        {
            // implemented in input function.
            // do nothing here.
            return;
        }
        else if (spr->id == 1)
        {
            // enemies travel left (-=)
            spr->x -= spr->speed;
        }
        else
        {
            // everything else travel right
            spr->x += spr->speed;
        }
    }
}

void update_bullet(Sprite *spr)
{
    int n,x,y;
    int x1,y1,x2,y2;
    //move the bullet
    update_sprite(spr);
    //check bounds

    if (spr->x > SCREEN_W)
    {
        spr->alive = 0;
        return;
    }

    for (n=0; n < MAX_ENEMIES; n++)
    {
        if (enemies[n]->alive)
        {
            //find center of bullet
            x = spr->x + spr->w/2;
            y = spr->y + spr->h/2;
            //get enemy plane bounding rectangle
            x1 = enemies[n]->x;
            y1 = enemies[n]->y;
            //y1 = enemies[n]->y - yoffset; ORIGINAL
            x2 = x1 + enemies[n]->w;
            y2 = y1 + enemies[n]->h;
            //check for collisions

            if (is_inside(x, y, x1, y1, x2, y2))
            {
                enemies_killed++;
                enemies[n]->alive=0;
                enemies[n]->dead=1;
                spr->alive=0;
                score++;
                break;
            }
        }
    }
}

void update_player()
{
    int n,x,y,x1=0,y1=0,x2=0,y2=0;

    //update/draw player sprite
    update_sprite(player);

    draw_sprite(buffer, player_bmp, player->x, player->y);

    //check for collision with enemies
    x = player->x + player->w/2;
    y = player->y + player->h/2;
    for (n=0; n<MAX_ENEMIES; n++)
    {
        if (enemies[n]->alive)
        {
            x1 = enemies[n]->x;
            y1 = enemies[n]->y;
            x2 = x1 + enemies[n]->w;
            y2 = y1 + enemies[n]->h;
        }
        if (is_inside(x,y,x1,y1,x2,y2))
        {
            enemies[n]->alive=0;
            enemies[n]->dead=1;
            score++;
        }
    }
}

void update_bullets()
{
    // draw bullets
    for(int i=0; i < MAX_BULLETS; i++)
    {
        if(bullets[i]->alive)
        {
            update_bullet(bullets[i]);
            draw_sprite(buffer, bullet_bmp,  bullets[i]->x, bullets[i]->y);
        }
    }
}

void update_enemies()
{
    for(int i = 0; i  < MAX_ENEMIES; i++)
    {
        if(enemies[i]->alive && !enemies[i]->dead)
        {
            // update existing enemies
            update_sprite(enemies[i]);
            draw_sprite_h_flip(buffer, enemy_bmp, enemies[i]->x, enemies[i]->y);
        }
        else if (!enemies[i]->dead)
        {
            // create new enemies with delay
            if(enemy_counter > enemy_delay)
            {
                enemy_counter = 0;
                enemy_count++;
                enemies[i]->alive++;
                return;
            }
        }
    }
}

int is_inside(int x,int y,int left,int top,int right,int bottom)
{

    if (x > left && x < right && y > top && y < bottom)
        return 1;
    else
        return 0;
}
