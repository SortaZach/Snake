// Actual Game logic goes here.
#include "snake.h"

g_internal int32_t
RoundReal32ToInt32(real32 Real32){
  int32_t Result = (int32_t)(Real32 + 0.5f);
  return(Result);
}

g_internal uint32_t
RoundReal32ToUInt32(real32 Real32) {
  int32_t Result = (int32_t)(Real32 + 0.5f);
  return (Result);
}

g_internal void
UpdateControlInput(game_controller_input Input) {
  if(Input.isAnalog) {
    // NOTE(Zach): use analog movement tuning
  } else {
  // NOTE(Zach): use digital movement tuning
  }
}

void
RenderPlayer(game_offscreen_buffer *Buffer, int PlayerX, int PlayerY) {
  uint8_t *endOfBuffer = (uint8_t *)Buffer->memory + Buffer->pitch* Buffer ->height;

  uint32_t Color = 0xFFFFFFFF;
  int top = PlayerY;
  int bottom = PlayerY+10;
  for( int x = PlayerX;
      x < PlayerX+10;
      ++x) {
    
    uint8_t *Pixel = ((uint8_t *)Buffer->memory +
                      x*Buffer->bytesPerPixel +
                      top*Buffer->pitch);

    for(int y = top;
        y < bottom;
        ++y) {
      
      if((Pixel >= (uint8_t*)Buffer->memory) && ((Pixel + 4) <= endOfBuffer)) {
        *(uint32_t *)Pixel = Color;
      }

      Pixel += Buffer->pitch;
    }
  }
}

g_internal void
GameUpdateAndRender( game_state *GameState, 
                  game_input *Input, 
                  game_offscreen_buffer *Buffer,
                  game_sound_output_buffer *SoundBuffer) {
  
  game_controller_input Input0 = Input->Controllers[0];
  if(Input0.moveUp.endedDown == 1 && Input0.moveUp.halfTransitionCount == 1){
    ErrorWindow("Move Up");
    Input0.moveUp.endedDown = 0;
  }

  if(Input0.moveDown.endedDown == 1 && Input0.moveDown.halfTransitionCount == 1){
    ErrorWindow("Move Down");
    Input0.moveUp.endedDown = 0;
  }

  if(Input0.moveLeft.endedDown == 1 && Input0.moveLeft.halfTransitionCount == 1){
    ErrorWindow("Move Left");
    Input0.moveUp.endedDown = 0;
    GameState->playerX += (int)(4.0f*1);
  }
  
  if(Input0.moveRight.endedDown == 1 && Input0.moveRight.halfTransitionCount == 1){
    ErrorWindow("Move Right");
    Input0.moveRight.endedDown = 0;
  }

  if(Input0.actionLeft.endedDown == 1 && Input0.actionLeft.halfTransitionCount == 1){
    ErrorWindow("Action Top");
    Input0.actionLeft.endedDown = 0;
  }
  
  if(Input0.actionDown.endedDown == 1 && Input0.actionDown.halfTransitionCount == 1){
    ErrorWindow("Action Down");
    Input0.actionDown.endedDown = 0;
  }

  if(Input0.actionLeft.endedDown == 1 && Input0.actionLeft.halfTransitionCount == 1){
    ErrorWindow("Action Left");
    Input0.actionLeft.endedDown = 0;
  }
  
  if(Input0.actionRight.endedDown == 1 && Input0.actionRight.halfTransitionCount == 1){
    ErrorWindow("Action Right");
    Input0.actionRight.endedDown = 0;
  }

  if(Input0.leftShoulder.endedDown == 1 && Input0.leftShoulder.halfTransitionCount == 1){
    ErrorWindow("Left Shoulder");
    Input0.leftShoulder.endedDown = 0;
  }

  if(Input0.rightShoulder.endedDown == 1 && Input0.rightShoulder.halfTransitionCount == 1){
    ErrorWindow("Right Shoulder");
    Input0.rightShoulder.endedDown = 0;
  }

  if(Input0.start.endedDown == 1 && Input0.start.halfTransitionCount == 1){
    ErrorWindow("Start");
    Input0.start.endedDown = 0;
  }

  if(Input0.back.endedDown == 1 && Input0.back.halfTransitionCount == 1){
    ErrorWindow("Back");
    Input0.back.endedDown = 0;
  }

  RenderPlayer(Buffer, GameState->playerX, GameState->playerY);
}
