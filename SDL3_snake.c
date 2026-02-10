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
          //TODO(zach): Set new gamepad for game.
        } break;

        case SDL_EVENT_GAMEPAD_REMOVED:
        {
          //TODO(zach): Close gamepad from game;
        }
      }
    }

      SDL_UpdateTexture(GlobalBackBuffer.texture, NULL, GlobalBackBuffer.memory, GlobalBackBuffer.pitch);
      SDL_RenderTexture(renderer, GlobalBackBuffer.texture, NULL, NULL);
      SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(GlobalBackBuffer.texture);
  SDL_free(GlobalBackBuffer.memory);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
