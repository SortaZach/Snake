// SDL3 Platform specific code.
#include "snake.h"
#include "SDL3_snake.h"
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

void 
RunHaptics(SDL_Haptic *haptic, float strength, int time_milli) {
  SDL_HapticID *haptics = SDL_GetHaptics(NULL);

  if (haptics) {
    haptic = SDL_OpenHaptic(haptics[0]);
    SDL_free(haptics);
  }

  if (haptic == NULL) {
    SDL_Log("Run Haptics: haptic was null");
    return;
  }

  if (haptic == NULL) return;

  SDL_InitHapticRumble(haptic);
  SDL_PlayHapticRumble(haptic, strength, time_milli);

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

  double fps_accum_seconds = 0.0;
  double work_accum_seconds = 0.0;
  uint32_t  fps_frames = 0;
  uint32_t missedFrames = 0;
  PerfFreq = SDL_GetPerformanceFrequency();
  uint64_t TargetCountsPerFrame = PerfFreq / GameUpdateHz;

  running = 1;

  //Play rumble at 50% for 2 seconds
  //RunHaptics(haptic, 0.50, 2000);

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


  if (gamepad) { SDL_CloseGamepad(gamepad); }
  if (haptic) { SDL_CloseHaptic(haptic); }

  SDL_DestroyTexture(GlobalBackBuffer.texture);
  SDL_free(GlobalBackBuffer.memory);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
