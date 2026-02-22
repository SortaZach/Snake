// Main Game Logic
int
intSignedToAbs(int value) {
  if (value < 0) {
    return value * -1;
  }
  return value;
}

void 
processTail(player *Player, tileMap TileMap) {
  if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_LEFT_TILE) {
    //NOTE (Zach): Clear Tile
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    //NOTE (Zach): Move Tail
    if (Player->tail_x_pos > 0) {
      --Player->tail_x_pos;
    } else {
      Player->tail_x_pos = (TILE_X_LENGTH - 1);
    }
  }

  else if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_RIGHT_TILE) {
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    if (Player->tail_x_pos < (TILE_X_LENGTH - 1)) {
      ++Player->tail_x_pos;
    } else {
      Player->tail_x_pos = 0;
    }
  }

  else if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_DOWN_TILE) {
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    if (Player->tail_y_pos < (TILE_Y_LENGTH - 1)) {
      ++Player->tail_y_pos;
    } else {
      Player->tail_y_pos = 0;
    }
  }

  else if(TileMap[Player->tail_x_pos][Player->tail_y_pos] == SNAKE_BODY_UP_TILE) {
    TileMap[Player->tail_x_pos][Player->tail_y_pos] = 0;
    if (Player->tail_y_pos > 0) {
      --Player->tail_y_pos;
    } else {
      Player->tail_y_pos = (TILE_Y_LENGTH) - 1;
    }
  }
}

void
movePlayer(player *Player, Sint16 axis_x, Sint16 axis_y, tileMap TileMap) {
  float horizontal_move = 0.0;
  float vertical_move = 0.0;
  int direction = 5;

  if (intSignedToAbs(axis_x) > 1000 || intSignedToAbs(axis_y) > 1000) {
    horizontal_move = axis_x / 32768.0;    
    vertical_move = axis_y / 32768.0;    
    
    if(intSignedToAbs(axis_x) >= intSignedToAbs(axis_y)) {
      if (horizontal_move > 0) {
        direction = DIRECTION_RIGHT;
      } else {
        direction = DIRECTION_LEFT;
      }
    } else {
      if (vertical_move > 0) {
        direction = DIRECTION_DOWN;
      } else {
        direction = DIRECTION_UP;
      }
    }
  }

  if ((direction == DIRECTION_RIGHT && Player->direction == DIRECTION_LEFT) || 
      (direction == DIRECTION_LEFT && Player->direction == DIRECTION_RIGHT) ||
      (direction == DIRECTION_UP && Player->direction == DIRECTION_DOWN)    || 
      (direction == DIRECTION_DOWN && Player->direction == DIRECTION_UP)    ||
      direction == 5) 
  {
    direction = Player->direction;
  }

  if(direction == DIRECTION_LEFT) {
    TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_LEFT_TILE;
    if (Player->head_x_pos > 0) {
      --Player->head_x_pos;
    } else {
      Player->head_x_pos = (TILE_X_LENGTH - 1);
    }
  }

  if(direction == DIRECTION_RIGHT) {
    TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_RIGHT_TILE;
    if (Player->head_x_pos < (TILE_X_LENGTH - 1)) {
      ++Player->head_x_pos;
    } else {
      Player->head_x_pos = 0;
    }
  }

  if (direction == DIRECTION_UP) {
    TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_UP_TILE;
    if (Player->head_y_pos > 0) {
      --Player->head_y_pos;
    } else {
      Player->head_y_pos = (TILE_Y_LENGTH - 1);
    }
  }

  if (direction == DIRECTION_DOWN) {
    TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_DOWN_TILE;
    if (Player->head_y_pos < (TILE_Y_LENGTH - 1)) {
      ++Player->head_y_pos;
    } else {
      Player->head_y_pos = 0;
    }
  }

  Player->direction = direction;
  if(Player->keep_tail == 1) {
    Player->keep_tail = 0;
  } else {
    processTail(Player, TileMap);
  }
}
