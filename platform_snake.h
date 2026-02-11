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


// NOTE(Zach):Four things the game needs to provide to the platform layer: Timing, controller/keyboard input, bitmap buffer to use, soundbuffer to use. 
// NOTE (Zach): Will probably  need  to  also add text buffer, and possibly other things as we go on. 

typedef struct game_offscreen_buffer {
  void *memory;
  int width;
  int height;
  int pitch;
  int bytesPerPixel;
} game_offscreen_buffer;

typedef struct game_sound_output_buffer {

} game_sound_output_buffer;


typedef struct game_button_state {
  int halfTransitionCount;
  bool32 endedDown;
} game_button_state;

typedef struct  game_controller_input {
  bool32 isConnected;
  bool32 isAnalog;
  real32 stickAverageX;
  real32 stickAverageY;

  union {
    game_button_state buttons[12];
    struct {
      game_button_state moveUp;
      game_button_state moveDown;
      game_button_state moveLeft;
      game_button_state moveRight;

      game_button_state actionUp;
      game_button_state actionDown;
      game_button_state actionLeft;
      game_button_state actionRight;
      
      game_button_state leftShoulder;
      game_button_state rightShoulder;

      game_button_state back;
      game_button_state start;

      //NOTE (zach): Use a terminator (must be last button in struct) for keeping track of number of items in struct.
      game_button_state terminator;
    };
  };
}  game_controller_input;

typedef struct game_input {
  game_button_state mouseButtons[5];
  int32_t mouseX, mouseY, mouseZ;

  real32 dtForFrame;
  game_controller_input Controllers[5];
} game_input;

typedef  struct game_memory  {
  bool32 isInitialized;

  uint64_t PermanentStorageSize;
  void *PermanentStorage; //NOTE(zach): REQUIRED to be zero at startup

  uint64_t transientStorageSize;
  void *transientStorage; //NOTE(zach): REQUIRED to be zero at startup
} game_memory;
#endif
