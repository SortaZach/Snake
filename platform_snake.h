#if !defined(SNAKE_PLATFORM_H)
/*=============================================================
Creator: Zachary Lecerf 
Notice: Copyright (C) 2026 Zach Lecerf <zach@dyforge.com>
===============================================================*/
#pragma once
#include  <stdint.h>

typedef uint32_t bool32;
typedef float real32;
typedef double real64;

#define global static
global int32_t WINDOW_WIDTH;
global int32_t WINDOW_HEIGHT;
global int32_t TILE_WIDTH;
global int32_t TILE_HEIGHT;
global int TILE_X_LENGTH = 16;
global int TILE_Y_LENGTH = 9;

typedef struct player {
  //void *texture;
  int head_x_pos;
  int head_y_pos;
  int tail_x_pos;
  int tail_y_pos;
  int keep_tail;
} player;

typedef struct food {
  //void *texture;
  int food_x;
  int food_y;
} food;

typedef struct game_state {
  int food_amount;
} game_state;

#endif
