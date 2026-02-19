// SDL3 Platform specific code.
#include "snake.h"
#include "SDL3_snake.h"


// TO PASS TO FRONT 
void
ErrorWindow (const char *error) {
  SDL_Log("%s", error); 
}

#include "snake.c"

// NOTE(Zach): Can change this to 60 is we want more animation room..  Can do a custom lock for menu screens seperate (like 15Hz) while the game could run at 60. Things to think about.
#define GameUpdateHz 30

global_variable int8_t running = 0;
global_variable sdl3_offscreen_buffer GlobalBackBuffer;
global_variable uint64_t PerfFreq = 0;

g_internal int InitEngine(){
  if  (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO | SDL_INIT_HAPTIC)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initalize SDL: %s", SDL_GetError());
    return 3;
  }

  return 0;
}

g_internal int
SDLGetWindowRefreshRate(SDL_Window *window) {
  int displayIndex = SDL_GetDisplayForWindow(window);
  int defaultRefreshRate = 60;

  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayIndex);

  if (mode->refresh_rate == 0) {
    SDL_Log("using default refresh rate %0.2f", mode->refresh_rate);
    return defaultRefreshRate;
  }
  return mode->refresh_rate;
}

g_internal real32
SDLGetSecondsElapsed(uint64_t OldCounter, uint64_t CurrentCounter) {
  return ((real32) (CurrentCounter - OldCounter) / (real32)PerfFreq);
}

g_internal void
SDLResizeBackBuffer(SDL_Renderer *Renderer, sdl3_offscreen_buffer *Buffer, int Width, int Height) {

  if (Buffer->texture) { 
    SDL_DestroyTexture(Buffer->texture);
    Buffer->texture = NULL;
  }

  if (Buffer->memory) { 
    SDL_free(Buffer->memory); 
    Buffer->memory = NULL;
  }

  Buffer->width = Width;
  Buffer->height = Height;
  Buffer->pitch = Width * 4;
  Buffer->memory = SDL_calloc(1, (size_t)Buffer->pitch * (size_t)Height);

  Buffer->texture = SDL_CreateTexture(Renderer,
                                    SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    Width, Height);

  SDL_SetTextureScaleMode(Buffer->texture, SDL_SCALEMODE_NEAREST);
}

g_internal void
SDLProccessButton(game_button_state *Old, game_button_state *New, bool32 IsDown) {
  if(New->endedDown != IsDown) {
    New->endedDown = IsDown;
    ++New->halfTransitionCount;
  }
}

bool32 IsDown = false;
g_internal void
SDLProccessKeyboardKeys(game_controller_input *Old, game_controller_input *New){
  const bool *key_states = SDL_GetKeyboardState(NULL);
  SDLProccessButton(&Old->moveUp, &New->moveUp, key_states[SDL_SCANCODE_W] | key_states[SDL_SCANCODE_UP]);
  SDLProccessButton(&Old->moveDown, &New->moveDown, key_states[SDL_SCANCODE_S] | key_states[SDL_SCANCODE_DOWN]);
  SDLProccessButton(&Old->moveLeft, &New->moveLeft, key_states[SDL_SCANCODE_A] | key_states[SDL_SCANCODE_LEFT]);
  SDLProccessButton(&Old->moveRight, &New->moveRight, key_states[SDL_SCANCODE_D] | key_states[SDL_SCANCODE_RIGHT]);
  SDLProccessButton(&Old->start, &New->start, key_states[SDL_SCANCODE_RETURN] | key_states[SDL_SCANCODE_P]);
  SDLProccessButton(&Old->back, &New->back, key_states[SDL_SCANCODE_DELETE] | key_states[SDL_SCANCODE_ESCAPE] | key_states[SDL_SCANCODE_BACKSPACE]);
  SDLProccessButton(&Old->actionDown, &New->actionDown, key_states[SDL_SCANCODE_SPACE]);
}

g_internal void
SDLProccessGameControllerButton(
      game_button_state *OldState,
      game_button_state *NewState,
      SDL_Gamepad *ControllerHandle,
      SDL_GamepadButton Button
    ) 
{
      NewState->endedDown = SDL_GetGamepadButton(ControllerHandle, Button);
      NewState->halfTransitionCount = ((NewState->endedDown == OldState->endedDown) ? 0 : 1);
}

void 
SDLRunHaptics(SDL_Haptic *haptic, float strength, int time_milli) {

  if (haptic == NULL) return;

  SDL_InitHapticRumble(haptic);
  SDL_PlayHapticRumble(haptic, strength, time_milli);
}


int main(int argc, char *argv[]){
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_AudioStream *audio_stream;
  SDL_Gamepad *gamepad;
  SDL_KeyboardID *keyboard;
  SDL_Haptic *haptic;
  SDL_Event event;

  char game_name[] = "Snake";
  char game_version[] = "v1.0.0";
  char game_identifier[] = "com.dyforge.snake";

  //NOTE(Zach): SDL3 uses this for info on apple etc
  SDL_SetAppMetadata(game_name, game_version, game_identifier);

  if (InitEngine() > 0) { return 1; }

  //NOTE(Zach): Many examples use SDL_CreateWindowAndRenderer() but for our purposes we're just going to create them seperate. 
  
  //---INITIALIZE  VALUES  ------
  window = SDL_CreateWindow("Snake", 1024, 768, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, NULL);

  //TODO(Zach): Change hardcoded Back buffer size to be screen size maybe?
  SDLResizeBackBuffer(renderer, &GlobalBackBuffer, 960, 540);

  gamepad = NULL; 
  keyboard = SDL_GetKeyboards(NULL);
  game_state GameState;

  //---INITALIZE HAPTICS ----
  haptic = NULL;
  SDL_HapticID *haptics = SDL_GetHaptics(NULL);
  if (haptics) {
    haptic = SDL_OpenHaptic(haptics[0]);
    SDL_free(haptics);
  }


  // --- INITIALIZE MEMORY ---
  game_memory GameMemory = {};
  GameMemory.permanentStorageSize = Megabytes(64);
  GameMemory.transientStorageSize = Gigabytes(4);
  uint64_t total = GameMemory.permanentStorageSize + GameMemory.transientStorageSize;

  void *base = SDL_calloc(1, total); // Allocate storage for all game memory.
                                     
  GameMemory.permanentStorage = base;
  GameMemory.transientStorage = (uint8_t *)base + GameMemory.permanentStorageSize;

  // --- INITIALIZE GAME INPUT ---
  game_input Input[2] = {0};
  game_input *NewInput = &Input[0];
  game_input *OldInput = &Input[1];


  //---INITIALIZE  PERFOMANCE WATCHING  ------
  double fps_accum_seconds = 0.0;
  double work_accum_seconds = 0.0;
  uint32_t  fps_frames = 0;
  uint32_t missedFrames = 0;
  PerfFreq = SDL_GetPerformanceFrequency();
  uint64_t TargetCountsPerFrame = PerfFreq / GameUpdateHz;

  running = 1;

  //Play rumble at 50% for 2 seconds
  //SDLRunHaptics(haptic, 0.50, 2000);


  if (GameMemory.permanentStorage && GameMemory.transientStorage) {

    while (running) {
      uint64_t frameStart = SDL_GetPerformanceCounter();

      while (SDL_PollEvent(&event)) {
        switch(event.type) {
          case SDL_EVENT_QUIT:
          {
            return SDL_APP_SUCCESS;
          } break;
          
          case SDL_EVENT_KEY_DOWN: 
          {
            if (event.key.key == SDLK_ESCAPE) {
              running = 0;
            }
          } break;

          case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
          {
            //TODO(zach): Add a ResizeBuffer Funciton
          } break;

          case SDL_EVENT_GAMEPAD_ADDED: 
          {
            if (gamepad == NULL) {
              gamepad = SDL_OpenGamepad(event.gdevice.which);

            //TODO(zach): Set Better Error Handling.
              if (gamepad == NULL) {
                SDL_Log("Error adding gamepad");
              }
            }
          } break;

          case SDL_EVENT_GAMEPAD_REMOVED:
          {
            if (gamepad && SDL_GetGamepadID(gamepad) == event.gdevice.which) {
              SDL_CloseGamepad(gamepad);
              gamepad = NULL;
            }
            //TODO(zach): Set Better Error Handling.
            if (gamepad == NULL) {
                SDL_Log("Error removing gamepad");
              }
          }
        }
      }
      
      if (keyboard) {
        game_controller_input *OldController = &OldInput->Controllers[0];
        game_controller_input *NewController = &NewInput->Controllers[0];

        *NewController = *OldController;

        NewController->moveUp.halfTransitionCount = 0;
        NewController->moveDown.halfTransitionCount = 0;
        NewController->moveLeft.halfTransitionCount = 0;
        NewController->moveRight.halfTransitionCount = 0;

        NewController->actionUp.halfTransitionCount = 0;
        NewController->actionDown.halfTransitionCount = 0;
        NewController->actionLeft.halfTransitionCount = 0;
        NewController->actionRight.halfTransitionCount = 0;
        
        NewController->start.halfTransitionCount = 0;
        NewController->back.halfTransitionCount = 0;

        NewController->leftShoulder.halfTransitionCount = 0;
        NewController->rightShoulder.halfTransitionCount = 0;

        SDLProccessKeyboardKeys(OldController, NewController);
      }

      if (gamepad) {
        game_controller_input *OldController = &OldInput->Controllers[0];
        game_controller_input *NewController = &NewInput->Controllers[0];


        //SDL_GAMEPAD_BUTTON_SOUTH // Bottom facing button (e.g. Xbox A button)
        //SDLProccessGameControllerButton(&(OldController->actionDown),
        //                         &(NewController->actionDown),
        //                         gamepad,
        //                         SDL_GAMEPAD_BUTTON_SOUTH);

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_NORTH)) {
          SDL_Log(" button north");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH)) {
          SDL_Log(" button south");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST)) {
          SDL_Log(" button east");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST)) {
          SDL_Log(" button west");
        }
   
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP)) {
          SDL_Log("DPad UP");
        }     

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)) {
          SDL_Log("DPad Down");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT)) {
          SDL_Log("DPad LEFT");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) {
          SDL_Log("DPad Right");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) {
          SDL_Log("Left bumper 1");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) {
          SDL_Log("RIGHT bumper 1");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START)) {
          SDL_Log("START");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_BACK)) {
          SDL_Log("BACK");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK)) {
          SDL_Log("Left Stick");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK)) {
          SDL_Log("Right Stick");
        }

        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_BACK)) {
          SDL_Log("BACK");
        }
        
        if(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)) {
          SDL_Log("Left Trigger");
        }

         if(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)) {
          SDL_Log("Right Trigger");
        }
         
        Sint16 right_stick_x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);

        Sint16 right_stick_y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);
      
        if(right_stick_x > 1000 || right_stick_y > 1000) {
          SDL_Log("right_gamestick");
        }

        Sint16 left_stick_x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);

        Sint16 left_stick_y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
      
        if(left_stick_x > 1000 || left_stick_y > 1000) {
          SDL_Log("left_gamestick");
        }
      }

      game_input *Temp = NewInput;
      OldInput = NewInput;
      NewInput = Temp;

      game_offscreen_buffer OffscreenBuffer;
      OffscreenBuffer.memory = GlobalBackBuffer.memory;
      OffscreenBuffer.width = GlobalBackBuffer.width;
      OffscreenBuffer.height = GlobalBackBuffer.height;
      OffscreenBuffer.pitch = GlobalBackBuffer.pitch;
      //ARGB8888 = 4 bytes per pixel
      OffscreenBuffer.bytesPerPixel = 4;

      game_sound_output_buffer SoundBuffer;

      GameUpdateAndRender(&GameState, NewInput, &OffscreenBuffer, &SoundBuffer);
      SDL_UpdateTexture(GlobalBackBuffer.texture, NULL, GlobalBackBuffer.memory, GlobalBackBuffer.pitch);
      SDL_RenderClear(renderer);
      SDL_RenderTexture(renderer, GlobalBackBuffer.texture, NULL, NULL);
      SDL_RenderPresent(renderer);

      //Measure work time not including sleep
      uint64_t workEnd = SDL_GetPerformanceCounter();
      uint64_t workCounts  = workEnd - frameStart;
      double workSeconds = (double)workCounts / (double)PerfFreq;

      if (workCounts < TargetCountsPerFrame) {
        uint64_t remaining = TargetCountsPerFrame - workCounts;
        uint64_t remainingNS = (remaining * 1000000000ULL) / PerfFreq;


        if (remaining > 1000000ULL) {
          SDL_DelayPrecise(remaining - 1000000ULL);
        }

        while ((SDL_GetPerformanceCounter() - frameStart) < TargetCountsPerFrame) {
          // Waiting..
        }


        uint64_t frameEnd = SDL_GetPerformanceCounter();
        double frameSeconds = (double)(frameEnd - frameStart) / (double)PerfFreq;

        fps_accum_seconds += frameSeconds;
        work_accum_seconds += workSeconds;
        fps_frames++;

        if (workCounts > TargetCountsPerFrame) {
          missedFrames++;
        }

        if (fps_accum_seconds >= 1.0) {
          double fps = (double)fps_frames / fps_accum_seconds;
          double avgTotalMs = 1000.0 * (fps_accum_seconds / (double)fps_frames);

          double avgWorkMs = 1000.0 * (work_accum_seconds / (double)fps_frames);

          double budgetMs = 1000.0 / (double)GameUpdateHz;
          double slackMs = budgetMs - avgWorkMs;

          SDL_Log("FPS: %5.1f | total: %6.3f ms | work:  %6.3f ms | slack: %6.3f ms | missed: %u", fps, avgTotalMs, avgWorkMs, slackMs,   missedFrames);

          fps_accum_seconds = 0.0;
          work_accum_seconds = 0.0;
          fps_frames = 0;
          missedFrames = 0;
        }
      }
    }
  }

  if (gamepad) { SDL_CloseGamepad(gamepad); }
  if (haptic) { SDL_CloseHaptic(haptic); }

  SDL_DestroyTexture(GlobalBackBuffer.texture);
  SDL_free(GlobalBackBuffer.memory);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
