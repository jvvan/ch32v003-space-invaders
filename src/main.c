#include "main.h"

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int main(void)
{
    // System init
    SystemInit();
    Delay_Ms(3000);

    // Display init
    tft_init();
    tft_set_background_color(BLACK);
    tft_clear();
    tft_set_color(PURPLE);

    // Controller init
    controller_init();

    // Main loop
    while (1)
    {
        int selected_option = title_screen();

        if (selected_option == 0)
        {
            game_screen();
        }
        else if (selected_option == 1)
        {
            credits_screen();
        }
    }

    printf("Stuck here forever...\n\r");
    while (1)
        ;
}

int title_screen()
{
    int TOP = ST7735_HEIGHT / 4;
    int LINE_GAP = 20;
    int selected_option = 0;
    char *options[] = {"Start", "Credits"};
    int options_len = 2;

    tft_clear();
    Delay_Ms(250);
    while (1)
    {
        tft_set_color(PURPLE);
        tft_print_align_center("Space Invaders", TOP);
        for (int i = 0; i < options_len; i++)
        {
            char option[20];
            if (i == selected_option)
            {
                tft_set_color(CYAN);
                sprintf(option, "> %s <", options[i]);
            }
            else
            {
                tft_set_color(PURPLE);
                sprintf(option, "  %s  ", options[i]);
            }

            tft_print_align_center(option, TOP + LINE_GAP * (i + 1));
        }

        controller_capture();
        if (controller_up_pressed(CTRL1) || controller_up_pressed(CTRL2))
        {
            selected_option = max(selected_option - 1, 0);
        }
        else if (controller_down_pressed(CTRL1) || controller_down_pressed(CTRL2))
        {
            selected_option = min(selected_option + 1, options_len - 1);
        }
        else if (controller_act_pressed(CTRL1) || controller_act_pressed(CTRL2))
        {
            return selected_option;
        }
    }
}

void game_reset(Game *game)
{
    game->enemy_bullet_start_spawn_time = get_current_time() + 1500;
    game->old_score = -1;
    game->old_lives = -1;

    game->enemy_right = 1;
    game->enemy_speed = 5;
    game->enemy_sprite = 0;

    game->player = (Player){
        .pos = {ST7735_WIDTH / 2, ST7735_HEIGHT - 20},
        .size = {16, 8},
        .vel = {0, 0},
        .bitmap = (uint8_t *)sprite_player,
    };

    game->bullet = (Bullet){
        .pos = {0, 0},
        .size = {3, 8},
        .vel = {0, 0},
        .active = 0,
        .bitmap = (uint8_t *)sprite_bullet_player,
    };

    for (int i = 0; i < ENEMY_BULLET_NUM; i++)
    {
        game->enemy_bullets[i] = (Bullet){
            .pos = {0, 0},
            .size = {3, 8},
            .vel = {0, 0},
            .active = 0,
            .bitmap = 0,
        };
    }

    // Lower the enemies each level
    int enemy_start_y = min(ST7735_HEIGHT - 20 - ENEMY_ROWS * 20, 20 + (game->level - 1) * 5);

    for (int i = 0; i < ENEMY_ROWS; i++)
    {
        for (int j = 0; j < ENEMY_COLS; j++)
        {
            game->enemies[i * ENEMY_COLS + j] = (Enemy){
                .pos = {j * 20 + 20, i * 20 + enemy_start_y},
                .size = {16, 8},
                .active = 1,
                .score = 30 - 10 * i,
            };

            if (i == 0)
            {
                game->enemies[i * ENEMY_COLS + j].sprite1 = (uint8_t *)sprite_enemy_11;
                game->enemies[i * ENEMY_COLS + j].sprite2 = (uint8_t *)sprite_enemy_12;
            }
            else if (i == 1)
            {
                game->enemies[i * ENEMY_COLS + j].sprite1 = (uint8_t *)sprite_enemy_21;
                game->enemies[i * ENEMY_COLS + j].sprite2 = (uint8_t *)sprite_enemy_22;
            }
            else if (i == 2)
            {
                game->enemies[i * ENEMY_COLS + j].sprite1 = (uint8_t *)sprite_enemy_31;
                game->enemies[i * ENEMY_COLS + j].sprite2 = (uint8_t *)sprite_enemy_32;
            }
        }
    }
}

void game_screen()
{
    tft_clear();

    Game game;
    game.lives = 3;
    game.level = 1;
    game.score = 0;
    game_reset(&game);

    srand(SysTick->CNT);

    while (1)
    {
        // Update inputs
        int64_t current_time = get_current_time();
        controller_capture();

        // Handle user input
        game.player.vel.x = game.player.vel.y = 0;

        if (controller_down_pressed(CTRL1))
        {
            game.player.vel.x -= 5;
        }
        if (controller_down_pressed(CTRL2))
        {
            game.player.vel.x += 5;
        }

        if (controller_up_pressed(CTRL1) || controller_up_pressed(CTRL2))
        {
            if (!game.bullet.active)
            {
                game.bullet.active = 1;
                game.bullet.pos.x = game.player.pos.x + game.player.size.x / 2 - game.bullet.size.x / 2;
                game.bullet.pos.y = game.player.pos.y - game.bullet.size.y;
                game.bullet.vel.x = 0;
                game.bullet.vel.y = -5;
            }
        }

        // Player movement

        // Make sure the velocity is within bounds

        if (game.player.vel.x < 0 && game.player.pos.x + game.player.vel.x < 0)
        {
            game.player.vel.x = -game.player.pos.x;
        }
        else if (game.player.vel.x > 0 && game.player.pos.x + game.player.size.x + game.player.vel.x > ST7735_WIDTH)
        {
            game.player.vel.x = ST7735_WIDTH - game.player.pos.x - game.player.size.x;
        }

        // Fill the old player position with black
        if (game.player.vel.x < 0)
        {
            tft_fill_rect(game.player.pos.x + game.player.size.x + game.player.vel.x, game.player.pos.y, -game.player.vel.x, game.player.size.y, BLACK);
        }
        else if (game.player.vel.x > 0)
        {
            tft_fill_rect(game.player.pos.x, game.player.pos.y, game.player.vel.x, game.player.size.y, BLACK);
        }

        // Update player position
        game.player.pos.x += game.player.vel.x;
        game.player.pos.y += game.player.vel.y;

        // Draw player
        tft_draw_bitmap(game.player.pos.x, game.player.pos.y, game.player.size.x, game.player.size.y, game.player.bitmap);

        // Player bullet logic
        if (game.bullet.active)
        {
            tft_fill_rect(game.bullet.pos.x, game.bullet.pos.y, game.bullet.size.x, game.bullet.size.y, BLACK);
            game.bullet.pos.y += game.bullet.vel.y;

            if (game.bullet.pos.y <= 16)
            {
                // Bullet out of bounds
                game.bullet.active = 0;
            }
            else
            {
                // Check if bullet hit an enemy
                uint8_t bullet_hit = 0;
                for (int i = 0; i < ENEMIES_NUM; i++)
                {
                    if (!game.enemies[i].active)
                    {
                        continue;
                    }
                    if (game.bullet.pos.x < game.enemies[i].pos.x + game.enemies[i].size.x &&
                        game.bullet.pos.x + game.bullet.size.x > game.enemies[i].pos.x &&
                        game.bullet.pos.y < game.enemies[i].pos.y + game.enemies[i].size.y &&
                        game.bullet.pos.y + game.bullet.size.y > game.enemies[i].pos.y)
                    {
                        // Bullet hit enemy
                        game.enemies[i].active = 0;
                        game.enemies[i].removed = 0;
                        game.enemies[i].dead_time = current_time;
                        game.bullet.active = 0;
                        game.score += game.enemies[i].score;
                        bullet_hit = 1;
                        tft_draw_bitmap(game.enemies[i].pos.x, game.enemies[i].pos.y, game.enemies[i].size.x, game.enemies[i].size.y, (uint8_t *)sprite_enemy_dead);

                        break;
                    }
                }

                if (bullet_hit)
                {
                    // Check if all enemies are dead
                    int all_dead = 1;
                    for (int j = 0; j < ENEMIES_NUM; j++)
                    {
                        if (game.enemies[j].active)
                        {
                            all_dead = 0;
                            break;
                        }
                    }

                    if (all_dead)
                    {
                        game.level++;
                        game.lives++;
                        tft_clear();
                        game_reset(&game);
                        continue;
                    }
                }
                else
                {
                    tft_draw_bitmap(game.bullet.pos.x, game.bullet.pos.y, game.bullet.size.x, game.bullet.size.y, game.bullet.bitmap);
                }
            }
        }

        // Enemy logic

        // Remove dead explosion sprites
        for (int i = 0; i < ENEMIES_NUM; i++)
        {
            if (!game.enemies[i].active && !game.enemies[i].removed && current_time - game.enemies[i].dead_time >= 100)
            {
                tft_fill_rect(game.enemies[i].pos.x, game.enemies[i].pos.y, game.enemies[i].size.x, game.enemies[i].size.y, BLACK);
                game.enemies[i].removed = 1;
            }
        }

        int enemies_should_move = (current_time - game.enemy_last_move) >= max(250, 1100 - game.level * 100);
        if (enemies_should_move)
        {
            game.enemy_last_move = current_time;

            // Clear old enemy positions
            for (int i = 0; i < ENEMIES_NUM; i++)
            {
                if (game.enemies[i].active)
                {
                    tft_fill_rect(game.enemies[i].pos.x, game.enemies[i].pos.y, game.enemies[i].size.x, game.enemies[i].size.y, BLACK);
                }
            }

            // Change enemy sprite
            game.enemy_sprite = !game.enemy_sprite;

            // Move enemies
            if (game.enemy_right)
            {
                int most_x = 0;
                for (int i = 0; i < ENEMIES_NUM; i++)
                {
                    if (game.enemies[i].active)
                    {
                        most_x = max(most_x, game.enemies[i].pos.x);
                    }
                }
                most_x += game.enemies[0].size.x;

                if (most_x + game.enemy_speed >= ST7735_WIDTH)
                {
                    game.enemy_right = 0;
                    for (int i = 0; i < ENEMIES_NUM; i++)
                    {
                        if (game.enemies[i].active)
                        {
                            game.enemies[i].pos.y += 5;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < ENEMIES_NUM; i++)
                    {
                        if (game.enemies[i].active)
                        {
                            game.enemies[i].pos.x += game.enemy_speed;
                        }
                    }
                }
            }
            else
            {
                int least_x = ST7735_WIDTH;
                for (int i = 0; i < ENEMIES_NUM; i++)
                {
                    if (game.enemies[i].active)
                    {
                        least_x = min(least_x, game.enemies[i].pos.x);
                    }
                }

                if (least_x - game.enemy_speed <= 0)
                {
                    game.enemy_right = 1;
                    for (int i = 0; i < ENEMIES_NUM; i++)
                    {
                        if (game.enemies[i].active)
                        {
                            game.enemies[i].pos.y += 5;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < ENEMIES_NUM; i++)
                    {
                        if (game.enemies[i].active)
                        {
                            game.enemies[i].pos.x -= game.enemy_speed;
                        }
                    }
                }
            }

            // Check if enemies reached the player
            int bottom_y = 0;
            for (int i = 0; i < ENEMIES_NUM; i++)
            {
                if (game.enemies[i].active)
                {
                    bottom_y = max(bottom_y, game.enemies[i].pos.y + game.enemies[i].size.y);
                }
            }

            if (bottom_y > game.player.pos.y)
            {
                // Player lost
                game_player_lost_screen(&game);
                return;
            }

            // Spawn enemy bullets
            if (current_time >= game.enemy_bullet_start_spawn_time)
            {
                int bullets_left = 0;
                for (int i = 0; i < ENEMY_BULLET_NUM; i++)
                {
                    bullets_left += !game.enemy_bullets[i].active;
                }

                for (int i = 0; i < ENEMIES_NUM; i++)
                {
                    if (game.enemies[i].active && bullets_left > 0 && rand() % 100 < 5)
                    {
                        for (int j = 0; j < ENEMY_BULLET_NUM; j++)
                        {
                            if (!game.enemy_bullets[j].active)
                            {
                                game.enemy_bullets[j].active = 1;
                                game.enemy_bullets[j].pos.x = game.enemies[i].pos.x + game.enemies[i].size.x / 2 - game.enemy_bullets[j].size.x / 2;
                                game.enemy_bullets[j].pos.y = game.enemies[i].pos.y + game.enemies[i].size.y / 2;
                                game.enemy_bullets[j].vel.x = 0;
                                game.enemy_bullets[j].vel.y = 3;
                                game.enemy_bullets[j].bitmap = rand() % 2 ? (uint8_t *)sprite_bullet_enemy_1 : (uint8_t *)sprite_bullet_enemy_2;
                                bullets_left--;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Draw enemies
        for (int i = 0; i < ENEMIES_NUM; i++)
        {
            if (game.enemies[i].active)
            {
                if (game.enemy_sprite == 0)
                {
                    tft_draw_bitmap(game.enemies[i].pos.x, game.enemies[i].pos.y, game.enemies[i].size.x, game.enemies[i].size.y, game.enemies[i].sprite1);
                }
                else
                {
                    tft_draw_bitmap(game.enemies[i].pos.x, game.enemies[i].pos.y, game.enemies[i].size.x, game.enemies[i].size.y, game.enemies[i].sprite2);
                }
            }
        }

        // Handle enemy bullets
        for (int i = 0; i < ENEMY_BULLET_NUM; i++)
        {
            if (game.enemy_bullets[i].active)
            {
                tft_fill_rect(game.enemy_bullets[i].pos.x, game.enemy_bullets[i].pos.y, game.enemy_bullets[i].size.x, game.enemy_bullets[i].size.y, BLACK);
                game.enemy_bullets[i].pos.y += game.enemy_bullets[i].vel.y;

                if (game.enemy_bullets[i].pos.y >= ST7735_HEIGHT)
                {
                    game.enemy_bullets[i].active = 0;
                }
                else
                {
                    if (game.enemy_bullets[i].pos.x < game.player.pos.x + game.player.size.x &&
                        game.enemy_bullets[i].pos.x + game.enemy_bullets[i].size.x > game.player.pos.x &&
                        game.enemy_bullets[i].pos.y < game.player.pos.y + game.player.size.y &&
                        game.enemy_bullets[i].pos.y + game.enemy_bullets[i].size.y > game.player.pos.y)
                    {
                        // Bullet hit player
                        game.enemy_bullets[i].active = 0;
                        game.lives--;
                        if (game.lives == 0)
                        {
                            // Player lost
                            game_player_lost_screen(&game);
                            return;
                        }
                        else
                        {
                            // Dead animation
                            uint8_t *animation[] = {(uint8_t *)sprite_player_dead_1, (uint8_t *)sprite_player_dead_2};
                            for (int i = 0; i < 10; i++)
                            {
                                tft_draw_bitmap(game.player.pos.x, game.player.pos.y, game.player.size.x, game.player.size.y, animation[i % 2]);
                                Delay_Ms(100);
                            }

                            // Cleanup old sprites
                            tft_fill_rect(game.player.pos.x, game.player.pos.y, game.player.size.x, game.player.size.y, BLACK);
                            for (int i = 0; i < ENEMY_BULLET_NUM; i++)
                            {
                                game.enemy_bullets[i].active = 0;
                                tft_fill_rect(game.enemy_bullets[i].pos.x, game.enemy_bullets[i].pos.y, game.enemy_bullets[i].size.x, game.enemy_bullets[i].size.y, BLACK);
                            }

                            game.enemy_bullet_start_spawn_time = get_current_time() + 1500;

                            break;
                        }
                    }
                    else
                    {
                        tft_draw_bitmap(game.enemy_bullets[i].pos.x, game.enemy_bullets[i].pos.y, game.enemy_bullets[i].size.x, game.enemy_bullets[i].size.y, game.enemy_bullets[i].bitmap);
                    }
                }
            }
        }

        // Score
        if (game.score != game.old_score)
        {
            game.old_score = game.score;
            tft_fill_rect(0, 0, ST7735_WIDTH / 2, 8, BLACK);
            tft_set_cursor(0, 0);
            tft_set_color(PURPLE);
            tft_print("Score: ");
            tft_print_number(game.score, 1);
        }

        // Lives
        if (game.lives != game.old_lives)
        {
            game.old_lives = game.lives;
            tft_fill_rect(ST7735_WIDTH / 2, 0, ST7735_WIDTH, 20, BLACK);

            int first_row = min(4, game.lives);
            int second_row = min(4, max(0, game.lives - 4));
            for (int i = 0; i < first_row; i++)
            {
                tft_draw_bitmap(ST7735_WIDTH - 20 * (i + 1), 0, game.player.size.x, game.player.size.y, game.player.bitmap);
            }
            for (int i = 0; i < second_row; i++)
            {
                tft_draw_bitmap(ST7735_WIDTH - 20 * (i + 1), 12, game.player.size.x, game.player.size.y, game.player.bitmap);
            }
        }

        Delay_Ms(TICK_TIME);
    }
}

void game_player_lost_screen(Game *game)
{
    char score_line[30];
    sprintf(score_line, "Score: %lu", game->score);
    tft_clear();
    tft_set_color(RED);
    tft_print_align_center("Game Over", ST7735_HEIGHT / 2 - 20);
    tft_print_align_center(score_line, ST7735_HEIGHT / 2);
    Delay_Ms(2000);
    tft_set_color(PURPLE);
    tft_print_align_center("Press any button to exit", ST7735_HEIGHT - 20);

    while (1)
    {
        controller_capture();
        if (controller_pad_pressed(CTRL1) || controller_pad_pressed(CTRL2))
        {
            return;
        }
    }
}

void credits_screen()
{
    tft_clear();
    tft_set_color(PURPLE);

    char *credits[] = {"Credits", "jvvan", "ch405labs", "ch32v003fun"};
    int credits_len = 4;
    int TOP = ST7735_HEIGHT / 6;

    for (int i = 0; i < credits_len; i++)
    {
        tft_print_align_center(credits[i], TOP + 20 * i);
        Delay_Ms(500);
    }

    tft_print_align_center("Press any button to exit", ST7735_HEIGHT - 20);
    while (1)
    {
        controller_capture();

        if (controller_pad_pressed(CTRL1) || controller_pad_pressed(CTRL2))
        {
            break;
        }
    }
}