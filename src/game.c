#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
game_t *create_default_game() {
  // TODO: Implement this function.
  game_t *def_game = malloc(sizeof(game_t));
  if (def_game == NULL){
    printf("Initialize Failed.");
    return NULL;
  }
  def_game->num_rows = 18;
  def_game->num_snakes = 1;
  def_game->snakes = malloc(def_game->num_snakes * sizeof(snake_t));
  if (def_game->snakes == NULL){
    printf("Snake Failed.");
    return NULL;
  }
  def_game->snakes->head_col = 4;
  def_game->snakes->head_row = 2;
  def_game->snakes->live = true;
  def_game->snakes->tail_col = 2;
  def_game->snakes->tail_row = 2;


  def_game->board = malloc(def_game->num_rows * sizeof(char *));
  if (def_game->snakes == NULL){
    printf("Board Initialize Failed.");
    return NULL;
  }

  def_game->board[0] = malloc(22 * sizeof(char));
  if (def_game->snakes == NULL){
    printf("Board Initialize Failed.");
    return NULL;
  }
  strcpy(def_game->board[0],"####################\n"); 

  def_game->board[def_game->num_rows - 1] = malloc(22 * sizeof(char));

  if (def_game->board[def_game->num_rows - 1] == NULL){
      printf("Board Allocate Failed.");
      return NULL;
  }
  strcpy(def_game->board[def_game->num_rows - 1],"####################\n"); 

  for(int i = 1; i < def_game->num_rows - 1; i++){
    def_game->board[i] = malloc(22 * sizeof(char));
    strcpy(def_game->board[i],"#                  #\n"); 

    if (def_game->board[i] == NULL){
      printf("Board Allocate Failed.");
      return NULL;
    }
  }
  strcpy(def_game->board[2],"# d>D    *         #\n");
  return def_game;
}

/* Task 2 */
void free_game(game_t *game) {
  // TODO: Implement this function.
  for(int i = 0; i < game->num_rows; i++){
    free(game->board[i]);
  }
  free(game->board);
  free(game->snakes);
  free(game);
  return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  // TODO: Implement this function.
  for(int i = 0; i < game->num_rows; i++){
    fprintf(fp,game->board[i],"%s");
  }
  return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if(c == 'w' || c == 'a' || c == 's' || c == 'd'){
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if(c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x'){
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if(c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x' || c == 'w' || c == 'a' || c == 's' || c == 'd'
  || c == '^' || c == 'v' || c == '<' || c == '>'){
    return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if(c == '^') return 'w';
  if(c == '<') return 'a';
  if(c == 'v') return 's';
  if(c == '>') return 'd';
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if(c == 'W') return '^';
  if(c == 'A') return '<';
  if(c == 'S') return 'v';
  if(c == 'D') return '>';
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if(c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if(c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if(c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if(c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  if (snum >= game->num_snakes) {printf("snum error.");return '?';}
  snake_t snek = game->snakes[snum];
  unsigned nextrow = get_next_row(snek.head_row , get_board_at(game , snek.head_row , snek.head_col));
  unsigned nextcol = get_next_col(snek.head_col , get_board_at(game , snek.head_row , snek.head_col));
  if(nextrow < game->num_rows && nextrow >= 0){
    return get_board_at(game,nextrow,nextcol);
  }
  return '?';
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  if (snum >= game->num_snakes) {printf("snum error.");return;}
  snake_t snek = game->snakes[snum];
  char curr_head = get_board_at(game , snek.head_row , snek.head_col);
  unsigned nextrow = get_next_row(snek.head_row , curr_head);
  unsigned nextcol = get_next_col(snek.head_col , curr_head);
  game->snakes[snum].head_row = nextrow;
  game->snakes[snum].head_col = nextcol;
  set_board_at(game, snek.head_row ,snek.head_col, head_to_body(curr_head));
  set_board_at(game, nextrow , nextcol , curr_head);
  return;
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  if (snum >= game->num_snakes) {printf("snum error.");return;}

  snake_t snek = game->snakes[snum];
  char curr_tail = get_board_at(game , snek.tail_row , snek.tail_col);
  unsigned nextrow = get_next_row(snek.tail_row , curr_tail);
  unsigned nextcol = get_next_col(snek.tail_col , curr_tail);

  game->snakes[snum].tail_row = nextrow;
  game->snakes[snum].tail_col = nextcol;
  set_board_at(game, snek.tail_row ,snek.tail_col, ' ');
  set_board_at(game, nextrow , nextcol , body_to_tail(get_board_at(game, nextrow, nextcol)));
  return;
}

/* Task 4.5 */
bool flaggg = false;
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  // TODO: Implement this function.
  if (game->num_snakes < 0) return;
  for(unsigned ui = 0; ui < game->num_snakes; ui++){
      if(flaggg == true){
        set_board_at(game, game->snakes[ui].head_row, game->snakes[ui].head_col,'x');
        game->snakes[ui].live = false; 
        flaggg = false;
      }
    if(game->snakes[ui].live == true){
      if(next_square(game,ui) == ' '){
        update_head(game,ui);
        update_tail(game,ui);
      }
      if((next_square(game,ui) == '#' || next_square(game,ui) == '^' || next_square(game,ui) == '<'
          || next_square(game,ui) == 'v' || next_square(game,ui) == '>' )){
            flaggg = true;
      }

      if(next_square(game,ui) == '*'){
        update_head(game,ui);
        add_food(game);
      }
    }  
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  char *s = malloc(99*sizeof(char));
  fgets(s,63,fp);
  char *fin =strchr(s,'\n');
  int index = (&fin - &s) / sizeof(char);
  char *news =(char*)realloc(s,index);

  return news;
}

/* Task 5.2 */
game_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  game_t* bboard = malloc(sizeof(game_t));
  bboard->num_snakes = 0;
  bboard->snakes = NULL;
  bboard->board = malloc(63 * sizeof(char*));
  unsigned flag = 0;
  while (!feof(fp)){
    char* str = read_line(fp);
    bboard->board[flag] = str;
    flag += 1;
  }
  flag -= 1;
  bboard->num_rows = flag;
  /*for(int j = flag + 1;j < 63; j++){
    free(bboard->board[flag]);
  }*/
  bboard->board =(char**)realloc(bboard->board,flag * sizeof(char*));
  return bboard;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  if (snum > game->num_snakes) {printf("snum error.");return;}

  unsigned curr_row = game->snakes[snum].tail_row;
  unsigned curr_col = game->snakes[snum].tail_col;
  char curr_char = get_board_at(game, curr_row, curr_col);

  while(is_snake(curr_char) && !is_head(curr_char)){
    curr_row = get_next_row(curr_row, curr_char);
    curr_col = get_next_col(curr_col, curr_char);
    curr_char = get_board_at(game, curr_row, curr_col);
  }

  game->snakes[snum].head_row = curr_row;
  game->snakes[snum].head_col = curr_col;
  return;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {
  // TODO: Implement this function.
  /*game->snakes = malloc(sizeof(snake_t));
  if (game->snakes == NULL){
    printf("Initialize Snake Failed.");
    return game;
  }*/
  game->num_snakes = 0;
  for(unsigned i = 0; i < game->num_rows; i++){
    unsigned len = 0;
    char ch = game->board[i][0];
    while(ch != '\n'){
      len++;
      ch = game->board[i][len];
    }
    for(unsigned j = 0; j < len ;j++){
      char cur = game->board[i][j];
      if(is_tail(cur)){
        game->snakes = malloc(sizeof(snake_t));
        if (game->snakes == NULL){
          printf("Initialize Snake Failed.");
          return game;
        }
        game->snakes[game->num_snakes].tail_row = i;
        game->snakes[game->num_snakes].tail_col = j;
        find_head(game,game->num_snakes);
        unsigned hi = game->snakes[game->num_snakes].head_row;
        unsigned hj = game->snakes[game->num_snakes].head_col;
        if(get_board_at(game, hi, hj) == 'x'){
          game->snakes[game->num_snakes].live = false;
        }
        else{game->snakes[game->num_snakes].live = true;}
        game->num_snakes++;
    }
  }
}
  return game;
}
