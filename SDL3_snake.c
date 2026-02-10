// SDL3 Platform specific code.
#include "SDL3_snake.h"

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14157265359f
#include "snake.c"

global_variable int8_t running = 0;
global_variable sdl3_offscreen_buffer GlobalBackBuffer;

internal int InitEngine(){
  if  (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO | SDL_INIT_HAPTIC)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initalize SDL: %s", SDL_GetError());
    return 3;
  }

  return 0;

}

void RunHaptics(SDL_Haptic *haptic, float strength, int time_milli) {
  SDL_HapticID *haptics = SDL_GetHaptics(NULL);
  if (haptics) {
    haptic = SDL_OpenHaptic(haptics[0]);
    SDL_free(haptics);
  }

  if (haptic == NULL) {
    SDL_Log("Run Haptics: haptic was null");
    return;
  }

  if (!SDL_InitHapticRumble(haptic)) {
    SDL_Log("InitHapticRumble: return was unexpected!");
    return;
  }

  if (!SDL_PlayHapticRumble(haptic, strength, time_milli)) {
    SDL_Log("PlayHapticRumble: return was unexpected!");
    return;
  }
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
  
  running = 1;


  //Play rumble at 50% for 2 seconds
  RunHaptics(haptic, 0.5, 2000);

  while (running) {
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
