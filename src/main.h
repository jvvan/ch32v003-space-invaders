#pragma once

#include "ch32v003fun.h"
#include "stdio.h"
#include "ch32v003_joypad.h"
#include "st7735.h"
#include "rand.h"
#include "time.h"
#include "resources/sprites.h"

#define tft_clear() tft_fill_rect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, BLACK)

#define ENEMY_ROWS 3
#define ENEMY_COLS 6
#define ENEMIES_NUM (ENEMY_ROWS * ENEMY_COLS)
#define ENEMY_BULLET_NUM 3
#define TICK_TIME 50

typedef struct Vector2
{
    int16_t x;
    int16_t y;
} Vector2;

typedef struct Player
{
    Vector2 pos;
    Vector2 size;
    Vector2 vel;
    uint8_t *bitmap;
} Player;

typedef struct Bullet
{
    Vector2 pos;
    Vector2 size;
    Vector2 vel;
    uint8_t *bitmap;
    uint8_t active;
} Bullet;

typedef struct Enemy
{
    Vector2 pos;
    Vector2 size;
    uint8_t *sprite1;
    uint8_t *sprite2;
    uint8_t active;
    uint8_t score;
    uint64_t dead_time;
    uint8_t removed;
} Enemy;

typedef struct Game
{
    Player player;
    Bullet bullet;
    Enemy enemies[ENEMY_ROWS * ENEMY_COLS];
    Bullet enemy_bullets[ENEMY_BULLET_NUM];
    uint32_t score;
    uint32_t old_score;
    uint32_t lives;
    uint32_t old_lives;
    uint32_t level;
    uint64_t enemy_bullet_start_spawn_time;
    uint8_t enemy_right;
    uint8_t enemy_speed;
    uint8_t enemy_sprite;
    uint64_t enemy_last_move;
} Game;

int title_screen();
void game_screen();
void game_player_lost_screen(Game *game);
void credits_screen();
int max(int a, int b);
int min(int a, int b);