// Actual Game logic goes here.
#include "snake.h"

g_internal void
GameUpdateAndRender( game_state *GameState, 
                  game_input *Input, 
                  game_offscreen_buffer *Buffer,
                  game_sound_output_buffer *SoundBuffer) {
  
  game_controller_input Input0 = Input->Controllers[0];
  if(Input0.isAnalog) {
    // NOTE(Zach): use analog movement tuning
  } 
  else
  {
    // NOTE(Zach): use digital movement tuning
  }

  if(Input0.moveUp.endedDown == 1){
    ErrorWindow("move Up");
  }

  if(Input0.actionUp.endedDown){
  }
}
