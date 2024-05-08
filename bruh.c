#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <math.h>

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
  char line[100];
} row_state;

typedef struct editorState {
  int fileposition_x, fileposition_y ;
  int cursorposition_x, cursorposition_y ;
  int screen_rows ;
  int screen_cols ;
  int no_of_lines ;
  FILE* fp ; 
}editorState ;

escseq CSI_code(){
  char ch ; 
  escseq escseq ;
  read(STDIN_FILENO, &ch, 1) ;//ignore
  // if(ch > 0 && ch <9) for(int i=0;i<ch;i++,read(STDIN_FILENO, &ch, 1));
  read(STDIN_FILENO, &ch, 1) ;
  switch(ch){
    case 'A' : return UP ;
    case 'B' : return DOWN ;
    case 'C' : return RIGHT ;
    case 'D' : return LEFT ;
    default : OTHER ;
  }
 
}

void nprintf(row_state* r_state){
  for(int i = 0 ; i < row_state->no_of_char ; i++){
    printf("%s", r_state->line) ;
    fflush(stdout) ;
  }
}

bool refresh_screen(editorState State, row_state* r_state, int bound ){
  printf("\e[1;1H") ;
  fflush(stdout) ;
  // for(int i = 0 ; i < bound ; i++){
    nprintf(r_state) ;
    // fflush(stdout);
  // }
  // printf("UPDATED CURSOR TO %d %d",State.cursorposition_y, State.cursorposition_x );
  printf("\e[%d;%dH", State.cursorposition_y+1, State.cursorposition_x+1);
  fflush(stdout);
  return 0 ;
}

void handle_CSI(editorState* State, escseq key) {
  switch(key) {
    case UP:
      // TODO: add bound chekcing
      State->fileposition_y--;
      State->cursorposition_y--;
      break;
    case DOWN:
      State->fileposition_y++;
      State->cursorposition_y++;
      break;
    case RIGHT:
      State->fileposition_x++;
      State->cursorposition_x++;
      break;
    case LEFT:
      State->fileposition_x--;
      State->cursorposition_x--;
      break;
  }
}
// SPECIAL KEY CSI
bool save_buffer(editorState* State, row_state* r_state){
    //27 special code
    char input;
    read(STDIN_FILENO, &input, 1);
    switch(input) {
      case '\e':
        escseq key = CSI_code();
        handle_CSI(State, key);
        break;
      case '\b':
        break;
      case '\n':
        // colm_real=0;
        // line_real++;
        // g[line_real] = calloc(100, sizeof(char));
        // lines = max(line_real, lines);
        break;
      default: 
         for(int i=89; i>=State->fileposition_x+1; i--) {
             r_state[State->fileposition_y].line[i] = r_state[State->fileposition_y].line[i-1];
          }
      if(isalnum(input) || isspace(input)) {
        r_state[State->fileposition_y].line[State->fileposition_x] = input;
        State->cursorposition_x++;
        State->fileposition_x++;
        r_state[State->fileposition_y].no_of_char++;
      }
    }
    return 1 ;
}

void get_window(int* rows, int* cols){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1){
      perror("ioctl") ;
    }
    *rows = ws.ws_row ;
    *cols = ws.ws_col ;
}

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


void disable_raw_mode(struct termios* original_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, original_termios);
}


void initEditor(editorState* State){
  State->fileposition_x = 0 ;
  State->fileposition_y = 0 ;
  State->cursorposition_x = 0 ;
  State->cursorposition_y = 0 ;
  get_window (&State->screen_rows, &State->screen_cols) ;
  //load the file
}


int main(int argc,char* argv[]){
  editorState State ;
  row_state *r_state = malloc(sizeof(row_state));
  bool changeflag = 0 ;
  enable_raw_mode() ;
  initEditor(&State) ;
  State.no_of_lines=1;
  while(1){
    char ch;
    // read(STDIN_FILENO, &ch, 1);
    // printf("%c", ch);
    // fflush(stdout);
    // if(changeflag) 
      changeflag = refresh_screen(State ,r_state, State.no_of_lines) ;
      changeflag = save_buffer(&State,r_state) ;
  }
  return 0 ;
}
