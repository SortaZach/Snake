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

s_internal int DIRECTION_UP    = 0;
s_internal int DIRECTION_DOWN  = 1;
s_internal int DIRECTION_LEFT  = 2;
s_internal int DIRECTION_RIGHT = 3;

s_internal uint32_t SNAKE_BASE_MOVEMENT = 150;


//NOTE (zach): Target Frame Rate
#define GameUpdateHz 30
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


  //--- INITALIZE PERFORMANCE WATCHING ---
  double fps_accum_seconds = 0.0;
  double work_accum_seconds = 0.0;
  uint32_t fps_frames = 0;
  uint64_t PerfFreq = SDL_GetPerformanceFrequency();
  uint64_t TargetCountsPerFrame = PerfFreq / GameUpdateHz;


  // --- SNAKE MOVEMENT ---

  // snake speed
  uint64_t last_counter = SDL_GetPerformanceCounter();
  double snake_step_seconds = 0.25;
  double snake_timer = 0.0;
  Sint16 axis_x, axis_y;

  // bitmapSurface = SDL_LoadBMP("img/hello.bmp");
  // bitmapTex = SDL_CreateTextureFromSurface();
  // SDL_DestroySurface(bitmapSurface);
  int8_t running = 1;
  while (running) {

    // Figure out when our frame begins
    uint64_t frame_start = SDL_GetPerformanceCounter();
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

      axis_x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
      axis_y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);


      float horizontal_move;
      float vertical_move;
    }

    // NOTE(Zach): Clear to a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    //NOTE (Zach): All body tiles are a int larger then 3 (the SNAKE_HEAD_TILE);
    if (TileMap[Player.head_x_pos][Player.head_y_pos] > SNAKE_HEAD_TILE) {
      running = 0;
    }

    SDL_FRect rect;
    // NOTE(Zach): Tile height and width should always be the same so that it's a square tile.
    TILE_WIDTH = WINDOW_WIDTH / TILE_X_LENGTH;
    TILE_HEIGHT = TILE_WIDTH;

    if(GameState.food_amount < 1) {
      uint8_t tile_occupied = 1;
      while(tile_occupied){
        Food.food_x = SDL_rand(TILE_X_LENGTH);
        Food.food_y = SDL_rand(TILE_Y_LENGTH);

        if( TileMap[Food.food_x][Food.food_y] < SNAKE_HEAD_TILE ){
          tile_occupied = false;
        } 
      }

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

    uint64_t work_end = SDL_GetPerformanceCounter();
    uint64_t work_counts = work_end - frame_start;
    double work_seconds = (double)work_counts / (double)PerfFreq;

    if (work_counts < TargetCountsPerFrame) {
      uint64_t remaining = TargetCountsPerFrame - work_counts;
      uint64_t remainingNS = (remaining * 1000000000ULL) / PerfFreq;

      if (remaining > 1000000ULL) {
        SDL_DelayPrecise(remaining - 1000000ULL);
      }

      while (( SDL_GetPerformanceCounter() - frame_start) < TargetCountsPerFrame ) {
        // TODO(zach): This should probably sleep or something
        // instead seeing as we're just using proccessing power for no reason... I think? 

        // Waiting for frame to finish..
      }

      // --- INTERNAL CLOCK FOR LOOP ---
      uint64_t now_counter = SDL_GetPerformanceCounter();
      double delta_seconds = (double)(now_counter - last_counter) / (double)PerfFreq;
      last_counter = SDL_GetPerformanceCounter();

      // Clamp
      if (delta_seconds > 0.25) delta_seconds = 0.25;

      // Increase timer
      snake_timer += delta_seconds;

      while (snake_timer >= snake_step_seconds) {
        getPlayerDirection(&Player, axis_x, axis_y, TileMap);
        snake_timer -= snake_step_seconds;    
      }

      /* ---- FOR CHECKING FPS | WORK | SLACK | MISSED ----  */
      uint64_t frame_end = SDL_GetPerformanceCounter();
      double frame_seconds = (double)(frame_end - frame_start) / (double)PerfFreq;
      int32_t missed_frames = 0;

      fps_accum_seconds += frame_seconds;
      work_accum_seconds += work_seconds;
      fps_frames++;

      if (work_counts > TargetCountsPerFrame) {
        missed_frames++;
      }

      if (fps_accum_seconds >= 1.0) {
        double fps = (double)fps_frames / fps_accum_seconds;
        double avg_total_ms = 1000.0 * (fps_accum_seconds / (double)fps_frames);
        double avg_work_ms = 1000.0 * (work_accum_seconds / (double)fps_frames);
        double budget_ms = 1000.0 / (double)GameUpdateHz;
        double slack_ms = budget_ms - avg_work_ms;

        SDL_Log("FPS: %5.1f | total: %6.3f ms | work:  %6.3f ms | slack: %6.3f ms | missed: %u", fps, avg_total_ms, avg_work_ms, slack_ms,   missed_frames);

        fps_accum_seconds = 0.0;
        work_accum_seconds = 0.0;
        fps_frames = 0;
        missed_frames = 0;
      }
    }

  }
  
  SDL_Quit();
  return 0;
}

// TODO(Zach): Different Screens need different things loaded
            /* 
             * First we need a menu:
             *                    - Extreme Mode 
             *                    - Classic Mode 
             *                    - Settings 
             *                    - Quit  
             *
             * Can use SDL_RenderTexture + 
             * BackBuffer For background and gameplay
             *
             * */
