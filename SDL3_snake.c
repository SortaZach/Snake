//SDL3 Platform Specific Layer
#include "SDL3_snake.h"

#define s_internal static 
s_internal int FOOD_TILE = 1;
s_internal int POWER_TILE = 2;
s_internal int SNAKE_HEAD_TILE = 3;
s_internal int SNAKE_BODY_UP_TILE = 4;
s_internal int SNAKE_BODY_RIGHT_TILE = 5;
s_internal int SNAKE_BODY_DOWN_TILE = 6;
s_internal int SNAKE_BODY_LEFT_TILE = 7;


typedef uint32_t tileMap[9][16];

void ProcessTail(tileMap TileMap, player *Player){
  if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_UP_TILE) {
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    Player->tail_y_pos -= 1;
  } 

  if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_DOWN_TILE) {
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    Player->tail_y_pos += 1;
  }

  if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_RIGHT_TILE) {
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    Player->tail_x_pos += 1;
  }

  if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_LEFT_TILE) {
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    Player->tail_x_pos -= 1;
  }
}

#include "snake.c"

int 
main(int argc, char *argv[]){
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_AudioStream *audio_stream;
  SDL_Gamepad *gamepad;
  SDL_KeyboardID *keyboard;
  SDL_Haptic *haptic;
  SDL_Event event;

  WINDOW_WIDTH  = 1920;
  WINDOW_HEIGHT = 1080;

  char game_name[] = "Snake";
  char game_version[] = "v0.0.1";
  char game_identifier[] = "com.dyforge.snake";
  
  SDL_SetAppMetadata(game_name, game_version, game_identifier);

  if( !( SDL_Init( SDL_INIT_VIDEO | 
                 SDL_INIT_GAMEPAD | 
                 SDL_INIT_AUDIO   | 
                 SDL_INIT_HAPTIC))) 
  {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initalize SDL: %s", SDL_GetError());
      return 3;
  }

  window = SDL_CreateWindow(game_name, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, NULL);
  gamepad = NULL;

  player Player;
  game_state GameState;
  food Food;
  GameState.food_amount = 0;
  Player.head_x_pos = 3;
  Player.head_y_pos = 3;
  Player.tail_x_pos = 3;
  Player.tail_y_pos = 3;
  tileMap TileMap;

  for (int i = 0; i < TILE_X_LENGTH; i++) { 
    for (int j = 0; j < TILE_Y_LENGTH; j++) { 
      TileMap[i][j] = 0; 
    } 
  }

  // NOTE (zach):
  // Use SDL_LOGICAL_PRESENTATION_LETTERBOX most of the time.
  // This is a unique case where we want to jump a tile 
  // Each movement for the snake, 
  // So scaling by Integer makes sense.
  SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

  // bitmapSurface = SDL_LoadBMP("img/hello.bmp");
  // bitmapTex = SDL_CreateTextureFromSurface();
  // SDL_DestroySurface(bitmapSurface);
  int running = 1;
  while (running) {
    while (SDL_PollEvent(&event)) {
      switch (event.type){
        case SDL_EVENT_KEY_DOWN:
        {
          if(event.key.key == SDLK_ESCAPE) {
            running = 0;
          }
        } break;

        case SDL_EVENT_QUIT: 
        {
          return SDL_APP_SUCCESS;
        } break;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        {
          SDL_GetWindowSizeInPixels(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
          SDL_DestroyRenderer(renderer);
          renderer = SDL_CreateRenderer(window, NULL);
          SDL_RenderClear(renderer);
        }

        case SDL_EVENT_GAMEPAD_ADDED:
        {
          if (gamepad == NULL) {
            gamepad = SDL_OpenGamepad(event.gdevice.which);
          }

          if (gamepad == NULL) {
            SDL_Log("Error adding gamepad");
          }
        } break;

        case SDL_EVENT_GAMEPAD_REMOVED:
        {

        } break;
      }
    }

    if (gamepad) {
      if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_NORTH)) {
        SDL_Log("button North");
      }

      if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH)) {
        SDL_Log("button South");
      }
      
      if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST)) {
        SDL_Log("button EAST");
        
      }

      if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST)) {
        SDL_Log("button WEST");
      }

      Sint16 axis_x, axis_y;
      axis_x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
      axis_y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);


      float horizontal_move;
      float vertical_move;
      getPlayerDirection(&Player, axis_x, axis_y, TileMap);
    }

    // NOTE(Zach): Clear to a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);


    SDL_FRect rect;

    // NOTE(Zach): Tile height and width should always be the same so that it's a square tile.
    TILE_WIDTH = WINDOW_WIDTH / TILE_X_LENGTH;
    TILE_HEIGHT = TILE_WIDTH;


    if(GameState.food_amount < 1) {
      Food.food_x = SDL_rand(TILE_X_LENGTH);
      Food.food_y = SDL_rand(TILE_Y_LENGTH);

      GameState.food_amount = 1;
    }

    TileMap[Food.food_x][Food.food_y] = FOOD_TILE;
    TileMap[Player.head_x_pos][Player.head_y_pos] = SNAKE_HEAD_TILE;

    if (Player.head_x_pos == Food.food_x && Player.head_y_pos == Food.food_y) {
      TileMap[Food.food_x][Food.food_y] = SNAKE_HEAD_TILE;
      GameState.food_amount = 0;
      Player.keep_tail = 2;
    }
    
    for(int i = 0; i < TILE_X_LENGTH; i++) {
      for (int j = 0; j < TILE_Y_LENGTH; j++) {
        rect.x = i * TILE_WIDTH;
        rect.y = j * TILE_HEIGHT;
        rect.w = TILE_WIDTH;
        rect.h = TILE_HEIGHT;

        if(TileMap[i][j] == 0) {
         SDL_SetRenderDrawColor(renderer, (i + j * 10), (i + j * 10), (i + j * 10), SDL_ALPHA_OPAQUE);
        } 

        else if(TileMap[i][j] == FOOD_TILE) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
        } 

        else if(TileMap[i][j] == POWER_TILE) {
          SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        }

        else if(TileMap[i][j] == SNAKE_HEAD_TILE) {
          SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
        }
        else if(TileMap[i][j] == SNAKE_BODY_UP_TILE   || 
                TileMap[i][j] == SNAKE_BODY_DOWN_TILE || 
                TileMap[i][j] == SNAKE_BODY_LEFT_TILE || 
                TileMap[i][j] == SNAKE_BODY_RIGHT_TILE) 
        {
          SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        } 
        else {
          SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        }

        SDL_RenderFillRect(renderer, &rect);
      }
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(150);
  }
  
  SDL_Quit();
  return 0;
}
