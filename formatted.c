#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

typedef enum escseq{
  OTHER ,
  UP ,
  DOWN ,
  RIGHT ,
  LEFT 
}escseq ;

typedef struct row_state{
  int no_of_char ;
  int line_no ;
  char* line ;
} row_state;

typedef struct editorState {
  int fileposition_x, fileposition_y ;
  int offset_x, offset_y ;
  int screen_rows ;
  int screen_cols ;
  int no_of_lines ;
  int *index_table ;
  FILE *fp ; 
} editorState ;

void render_headder(editorState State) {
    printf("\033[1;1H"); 
    printf("\033[47m \033[30;47m");
    printf("  Spring Syntax");
    for(int j=0; j<State.screen_cols-16; j++) printf(" ");
    printf("\033[1;1H\033[1B\033[1B\033[0m");
    fflush(stdout);
}

void render_footer(editorState State) {
    printf("\033[%d;%dH", State.screen_rows-1, 1); 
    printf("\033[47m \033[30;47m");
    printf(" Ins | line %d col %d", State.fileposition_y, State.fileposition_x);
    for(int j=0; j<State.screen_cols-25; j++) printf(" ");
    printf("\033[1;1H\033[0m");
    fflush(stdout);
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

row_state* get_row_at(editorState *State, row_state *r_state, int position) {
  return r_state+ State->index_table[State->fileposition_y];
} 

void newline(editorState* State, row_state** r_state){

  int pos_y = (State->fileposition_y);
  int pos_x = (State->fileposition_x);

  if(State->no_of_lines%10 == 0){
    *r_state = realloc(*r_state, sizeof(row_state)*(State->no_of_lines+10)) ;
    State->index_table = realloc(State->index_table, sizeof(int) * (State->no_of_lines +10));
  }
  
  row_state *new_row =  *r_state + State->no_of_lines;
  row_state *curr_row = get_row_at(State, *r_state, pos_y);
  // Allocate space for next line
  new_row->line = malloc(sizeof(char) * 100);
  new_row->no_of_char = curr_row->no_of_char - pos_x;
  new_row->line_no = pos_y + 1 + 1;

  // Copy the text after the cursor to new line
  memcpy(new_row->line, curr_row->line+pos_x, new_row->no_of_char+1);

  curr_row->no_of_char = pos_x;
  curr_row->line[pos_x] = 0;

  // Update the state
    // update the index table
  pos_y++;
  for(int i=State->no_of_lines; i> pos_y ; i--)
     State->index_table[i] = State->index_table[i-1];
  State->index_table[pos_y] = State->no_of_lines;

  State->no_of_lines++;
  State->fileposition_x= 0;
  for(int i=0; i<State->no_of_lines-1; i++) {
    if((*r_state+i)->line_no >= new_row->line_no) {
      (*r_state+i)->line_no++;
    }
  }
  State->fileposition_y++;
}


void clear_display(){
  printf("\e[2J") ;
}

void cursor_to(int y,int x){
  printf("\e[%d;%dH", y, x);
}

escseq CSI_code(){
  char ch ; 
  escseq escseq ;
  read(STDIN_FILENO, &ch, 1) ;//ignore
  read(STDIN_FILENO, &ch, 1) ;
  switch(ch){
    case 'A' : return UP ;
    case 'B' : return DOWN ;
    case 'C' : return RIGHT ;
    case 'D' : return LEFT ;
    default : return OTHER ;
  }
 }


void handle_CSI(editorState* State, escseq key, row_state * r_state) {
  switch(key) {
    case UP:
      if(State->fileposition_y != 0){ // currently not on the first line  
        State->fileposition_y--;
        if(State->fileposition_x > r_state[State->index_table[State->fileposition_y]].no_of_char){
          State->fileposition_x = r_state[State->index_table[State->fileposition_y]].no_of_char;    // set the x in bound if out of bound after going down in line
        }
      }
      break;
    case DOWN:
      
      if(State->fileposition_y != State->no_of_lines-1){ // currently not on the last line  
        State->fileposition_y++;
        if(State->fileposition_x > r_state[State->index_table[State->fileposition_y]].no_of_char){
          State->fileposition_x = r_state[State->index_table[State->fileposition_y]].no_of_char;    // set the x in bound if out of bound after going down in line
        }
      }
      break;
    case RIGHT:
      if(State->fileposition_x == r_state[State->index_table[State->fileposition_y]].no_of_char ){      
              
           if(State->fileposition_y + 1 <= State->no_of_lines-1 ){  // next line exists
             State->fileposition_y++;
             State->fileposition_x = 0;
            }         
      }else{ // cursor was between somewhere in the line
        State->fileposition_x++;
      }
      break;
      
    case LEFT:
      //State->fileposition_x--;
      if(State->fileposition_x == 0){  // check if cursor was at the beginning of line            
         // go to the above line last char
         if(State->fileposition_y - 1 >= 0 ){  // prev line exists
           State->fileposition_y--;
           State->fileposition_x = r_state[State->index_table[State->fileposition_y]].no_of_char;
        }
      }else{ // cursor was between somewhere in the line
        State->fileposition_x--;
      }
      break;
  }
}

void backSpace(editorState *State , row_state *r_state){
  char temp = r_state->line[State->fileposition_x];
  if(State->fileposition_x <= 0) return;
  for( int i = State->fileposition_x -1; i < r_state->no_of_char ; i++){
    r_state->line[i]=r_state->line[i+1];
    if(r_state->no_of_char==i){
      r_state->line[i]='\0';
      break;
    }
  }
  r_state->no_of_char--;
  State->fileposition_x--;
}

bool save_buffer(editorState *State, row_state **r_state){
  
    char input;
    read(STDIN_FILENO, &input, 1);
    switch(input) {
      case '\e':
        escseq key = CSI_code();
        handle_CSI(State, key, *r_state);
        break;
      case 127:
            backSpace(State , get_row_at(State, *r_state, State->fileposition_y));
        break;
      case '\n':

        newline(State, r_state);
        // int pos_y = ++(State->fileposition_y);
        // int pos_x = (State->fileposition_x);

        // row_state *new_row = r_state + State->no_of_lines;
        // row_state *curr_row = r_state + pos_y - 1;
      
        // // Allocate space for next line
        // new_row->line = malloc(sizeof(char) * 100);
        // new_row->no_of_char = curr_row->no_of_char - pos_x +1;
        // new_row->line_no = pos_y + 1;

        // // Copy the text after the cursor to new line
        // memcpy(new_row->line, curr_row->line+pos_x, new_row->no_of_char+1);
      
      
        // curr_row->no_of_char = pos_x;
        // curr_row->line[pos_x] = 0;

        // // Update the state
        // State->no_of_lines++;
        // State->fileposition_x= 0;
        // for(int i=0; i<State->no_of_lines-1; i++) {
        //   if((r_state+i)->line_no >= new_row->line_no) {
        //     (r_state+i)->line_no++;
        //   }
        // }
        // State->fileposition_y=State->no_of_lines-1;
     
        break;
      default: 
         for(int i=89; i>=State->fileposition_x+1; i--) {
             (*r_state)[State->index_table[State->fileposition_y]].line[i] = (*r_state)[State->index_table[State->fileposition_y]].line[i-1];
          }
        // r_state[State->fileposition_y].line[State->fileposition_x] = input;
        get_row_at(State, *r_state, State->fileposition_y)->line[State->fileposition_x] = input;
        State->fileposition_x++;
        // r_state[State->fileposition_y].no_of_char++;
        get_row_at(State, *r_state, State->fileposition_y)->no_of_char++;
    }
    return 1 ;
}

void move_cursor_to_home() {
  printf("\e[2;1H") ;
}

void nprintf(row_state *r_state, editorState State){
  move_cursor_to_home() ;
  cursor_to(r_state->line_no + 2, 4);
  printf("\e[0K") ;
  printf("\e[1;30m%d\t", r_state->line_no);
  if(State.fileposition_y+1 == r_state->line_no) printf("|");
  else printf(" ");
  printf(" \e[1;0m");
  fflush(stdout) ;
  for(int i = 0 ; i < r_state->no_of_char ; i++){
    printf("%c", r_state->line[i]) ;
    fflush(stdout) ;
  }
  printf("\e[0K") ;
  fflush(stdout) ;
}

bool refresh_screen(editorState State, row_state *r_state, int bound ){
  // clear_display() ;
  render_headder(State);
  render_footer(State);
  move_cursor_to_home() ;
  printf("\e[?25l");
  for(int i = 0 ; i < bound ; i++){
    nprintf(r_state+i, State) ;
  }
  cursor_to(State.fileposition_y+3, State.fileposition_x+11) ;
  printf("\e[?25h");
  fflush(stdout);
  return 0 ;
}

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void get_window(int *rows, int *cols){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1){
      perror("ioctl") ;
    }
    *rows = ws.ws_row ;
    *cols = ws.ws_col ;
}

void initEditor(editorState* State){
  State->fileposition_x = 0 ;
  State->fileposition_y = 0 ;
  State->offset_x = 0 ; 
  State->offset_y = 0 ;
  State->no_of_lines = 1 ;;
  get_window (&State->screen_rows, &State->screen_cols) ;
  clear_display();
  fflush(stdout);
  //load the file
}

int main(int argc,char *argv[]){
  editorState State ;
  row_state *r_state = malloc(sizeof(row_state) * 10);
  State.index_table  = malloc(sizeof(int) * 10);
  for(int i=0; i < 10; i++) State.index_table[i] = i;
  r_state->line = malloc(sizeof(char)*1000);
  r_state->line_no = 1;
  bool changeflag = 1 ;
  enable_raw_mode() ;
  initEditor(&State) ;
  while(1){
    if(changeflag)
      changeflag = refresh_screen(State ,r_state, State.no_of_lines) ;
    changeflag = save_buffer(&State, &r_state) ;
  }
  return 0 ;
}
