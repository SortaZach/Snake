// SDL3 Platform specific code.

#include "snake.h"
#include "SDL3_snake.h"
#include "snake.c"

global_variable int8_t running = 0;
global_variable sdl3_offscreen_buffer GlobalBackBuffer;
real32 TargetSecondsPerFrame = 1.0f / (real32)GameUpdateHz;

g_internal int InitEngine(){
  if  (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO | SDL_INIT_HAPTIC)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initalize SDL: %s", SDL_GetError());
    return 3;
  }

  return 0;
}

void 
RunHaptics(SDL_Haptic **haptic, float strength, int time_milli) {
  SDL_HapticID *haptics = SDL_GetHaptics(NULL);

  if (haptics) {
    *haptic = SDL_OpenHaptic(haptics[0]);
    SDL_free(haptics);
  }

  if (haptic == NULL) {
    SDL_Log("Run Haptics: haptic was null");
    return;
  }

  if (*haptic == NULL) return;

  SDL_InitHapticRumble(*haptic);
  SDL_PlayHapticRumble(*haptic, strength, time_milli);

}

g_internal int
SDLGetWindowRefreshRate(SDL_Window *window) {
  int displayIndex = SDL_GetDisplayForWindow(window);
  int defaultRefreshRate = 60;

  // NOTE(Zach): SDL_GetDesktopDisplayMode is used when not wanting to change 
  // the native display mode and not the current display mode. (I think)
  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(displayIndex);

  if (mode->refresh_rate == 0) {
    return defaultRefreshRate;
  }
  SDL_Log("refresh rate %0.2f", mode->refresh_rate);
  return mode->refresh_rate;
}


global_variable uint64_t PerfFreq = 0;

g_internal real32
SDLGetSecondsElapsed(uint64_t OldCounter, uint64_t CurrentCounter) {
  return ((real32) (CurrentCounter - OldCounter) / (real32)PerfFreq);
}

int main(int argc, char *argv[]){
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_AudioStream *audio_stream;
  SDL_Gamepad *gamepad;
  SDL_Haptic *haptic;
  SDL_Event event;

  char game_name[] = "Snake";
  char game_version[] = "v1.0.0";
  char game_identifier[] = "com.dyforge.snake";

  //NOTE(Zach): SDL3 uses this for info on apple etc
  SDL_SetAppMetadata(game_name, game_version, game_identifier);

  if (InitEngine() > 0) { return 1; }

  //NOTE(Zach): Many examples use SDL_CreateWindowAndRenderer() but for our purposes we're just going to create them seperate. 
  window = SDL_CreateWindow("Snake", 1024, 768, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, NULL);
  gamepad = NULL; 
  haptic = NULL;
  PerfFreq = SDL_GetPerformanceFrequency();

  running = 1;

  //Play rumble at 50% for 2 seconds
  RunHaptics(haptic, 0.25, 1000);

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

    if (gamepad) {

    }

    SDL_UpdateTexture(GlobalBackBuffer.texture, NULL, GlobalBackBuffer.memory, GlobalBackBuffer.pitch);
    SDL_RenderTexture(renderer, GlobalBackBuffer.texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    uint64_t frameEnd = SDL_GetPerformanceCounter();
    uint64_t elapsed = SDLGetSecondsElapsed(frameStart, frameEnd);

    if (elapsed < TargetSecondsPerFrame) {
      uint64_t remaining = TargetSecondsPerFrame - elapsed;

      if (remaining > 1000000ULL) {
        SDL_DelayPrecise(remaining - 1000000ULL);
      }

      while ((SDL_GetPerformanceCounter() - frameStart) < TargetSecondsPerFrame) {
        // Waiting..
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
