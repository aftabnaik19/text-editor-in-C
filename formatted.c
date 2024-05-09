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
  FILE* fp ; 
}editorState ;


int min(int a, int b) {
    return (a < b) ? a : b;
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
    default : OTHER ;
  }
 
}

void handle_CSI(editorState* State, escseq key) {
  switch(key) {
    case UP:
      // TODO: add bound chekcing
      State->fileposition_y--;
      break;
    case DOWN:
      State->fileposition_y++;
      break;
    case RIGHT:
      State->fileposition_x++;
      break;
    case LEFT:
      State->fileposition_x--;
      break;
  }
}

void backSpace(editorState* State , row_state *r_state){
  char temp = r_state->line[State->fileposition_x];
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

bool save_buffer(editorState* State, row_state* r_state){
  
    char input;
    read(STDIN_FILENO, &input, 1);
    switch(input) {
      case '\e':
        escseq key = CSI_code();
        handle_CSI(State, key);
        break;
      case 127:
            backSpace(State ,r_state+State->fileposition_y);
        break;
      case '\n':
      
        int pos_y = 1+(State->fileposition_y);
        int pos_x = (State->fileposition_x);
        State->fileposition_y++;

        row_state *new_row = r_state + pos_y;
        row_state *curr_row = r_state + pos_y - 1;
      
        // Allocate space for next line
        new_row->line = malloc(sizeof(char) * 100);
        new_row->no_of_char = curr_row->no_of_char - pos_x +1;
        new_row->line_no = pos_y + 1;

        // Copy the text after the cursor to new line
        memcpy(new_row->line, curr_row->line+pos_x, new_row->no_of_char+1);
      
      
        curr_row->no_of_char = pos_x;
        curr_row->line[pos_x] = 0;

        // Update the state
        State->no_of_lines++;
        State->fileposition_x= 0;
      
        break;
      default: 
         for(int i=89; i>=State->fileposition_x+1; i--) {
             r_state[State->fileposition_y].line[i] = r_state[State->fileposition_y].line[i-1];
          }
        r_state[State->fileposition_y].line[State->fileposition_x] = input;
        State->fileposition_x++;
        r_state[State->fileposition_y].no_of_char++;
    }
    return 1 ;
}

void nprintf(row_state* r_state){
  for(int i = 0 ; i < r_state->no_of_char ; i++){
    printf("%c", r_state->line[i]) ;
    fflush(stdout) ;
  }
  printf("\n") ;
  fflush(stdout) ;
}

bool refresh_screen(editorState State, row_state* r_state, int bound ){
  clear_display() ;
  printf("\e[2;1H") ;//added space for header
  fflush(stdout) ;
  for(int i = 0 ; i < bound ; i++){
    nprintf(r_state+i) ;
    fflush(stdout);
  }
  cursor_to(State.fileposition_y+2,State.fileposition_x+1) ;
  fflush(stdout);
  return 0 ;
}

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void get_window(int* rows, int* cols){
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
  //load the file
}

int main(int argc,char* argv[]){
  editorState State ;
  row_state *r_state = malloc(sizeof(row_state)*10);
  r_state->line = malloc(sizeof(char)*1000);
  r_state->line_no = 1;
  bool changeflag = 0 ;
  enable_raw_mode() ;
  initEditor(&State) ;
  while(1){
    if(changeflag)
      changeflag = refresh_screen(State ,r_state, State.no_of_lines) ;
      changeflag = save_buffer(&State,r_state) ;
  }
  return 0 ;
}
