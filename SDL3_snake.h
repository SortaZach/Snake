#if !defined(SDL3_SNAKE_H)
/*=============================================================
Creator: Zachary Lecerf 
Notice: Copyright (C) 2026 Zach Lecerf <zach@dyforge.com>
===============================================================*/
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gamepad.h>
#include <x86intrin.h>

typedef struct sdl3_offscreen_buffer {
  SDL_Texture *texture;
  void *memory;
  int height;
  int width;
  int pitch;
} sdl3_offscreen_buffer;

typedef struct text_buffer {
  int size_of;
} text_buffer;

typedef struct audio_stream_buffer {
  int samples;
} audio_stream_buffer;

#endif
