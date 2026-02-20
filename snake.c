// Main Game Logic
int
intSignedToAbs(int value) {
  if (value < 0) {
    return value * -1;
  }
  return value;
}

void
getPlayerDirection(player *Player, Sint16 axis_x, Sint16 axis_y, tileMap TileMap) {
 float horizontal_move;
 float vertical_move;
  if (intSignedToAbs(axis_x) > 1000 || intSignedToAbs(axis_y) > 1000) {
    horizontal_move = axis_x / 32768.0;
    vertical_move = axis_y / 32768.0;
    
    if(intSignedToAbs(axis_x) >= intSignedToAbs(axis_y)) {
      if (horizontal_move > 0){
        TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_RIGHT_TILE;

        if (Player->head_x_pos < (TILE_X_LENGTH - 1)) {
          ++Player->head_x_pos;
        } else {
          Player->head_x_pos = 0;
        }
      } 
      
      if (horizontal_move < 0) {
        TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_LEFT_TILE;

        if (Player->head_x_pos > 0) {
          --Player->head_x_pos;
        } else {
          Player->head_x_pos = (TILE_X_LENGTH - 1);
        }
      }
    } else {
      if (vertical_move > 0){
        TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_DOWN_TILE;
        if (Player->head_y_pos < (TILE_Y_LENGTH - 1)) {
          ++Player->head_y_pos;
        } else {
          Player->head_y_pos = 0;
        }
      }

      if (vertical_move < 0){
        TileMap[Player->head_x_pos][Player->head_y_pos] = SNAKE_BODY_UP_TILE;
        if (Player->head_y_pos > 0) {
          --Player->head_y_pos;
        } else {
          Player->head_y_pos = (TILE_Y_LENGTH - 1);
        }
      }
    }
    if(Player->keep_tail <= 0){
      ProcessTail(TileMap, Player);
      Player->keep_tail = 0;
    } else {
      Player->keep_tail -= 1;
    }
  }  
}

