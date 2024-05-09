#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

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
    int screen_rows ;
    int screen_cols ;
    int no_of_lines ;
    FILE* fp ; 
  }editorState ;


void printCenteredString(int windowHeight, int windowWidth, const char *str) {
      int len = strlen(str);  // Calculate the length of the string
      // Print the left padding
      int leftPadding = (windowWidth - len) / 2;  // Calculate left padding
      leftPadding = (leftPadding < 0) ? 0 : leftPadding;
      for (int i = 0; i < leftPadding; i++) {
            printf(" ");
        }
  }


void character_art(int window_height,int window_width){

        printCenteredString(window_height,window_width, " #####   ###  ### ##   ##  ########   ###    ###  ##            #####   ######    ##  ##  #######  ######   #######");
      printCenteredString(window_height,window_width, "##   ##   ##  ##  ###  ##  ## ## ##  ## ##   ###  ##           ##   ##   ##  ##   ##  ##   ##   #   ##  ##   ##   #");
      printCenteredString(window_height,window_width, "##         ####   #### ##     ##    ##   ##   #####            ##        ##  ##   ##  ##   ##       ##  ##   ##    ");
      printCenteredString(window_height,window_width, " #####      ##    #######     ##    ##   ##    ###              #####    #####    ######   ####     #####    ####  ");
      printCenteredString(window_height,window_width, "     ##     ##    ## ####     ##    #######   #####                 ##   ##       ##  ##   ##       ## ##    ##    ");
      printCenteredString(window_height,window_width, "##   ##     ##    ##  ###     ##    ##   ##  ##  ###           ##   ##   ##       ##  ##   ##   #   ## ##    ##   #");
      printCenteredString(window_height,window_width, " #####     ####   ##   ##    ####   ##   ##  ##  ###            #####   ####      ##  ##  #######  #### ##  #######");
  }

void put_in_line(editorState* State, row_state* r_state, char input){
     r_state[State->fileposition_y].line[State->fileposition_x] = input;
     State->fileposition_x++;
     r_state[State->fileposition_y].no_of_char++;
  }

escseq CSI_code(){
    char ch ; 
    escseq escseq ;
    read(STDIN_FILENO, &ch, 1) ;//ignore
    if(ch > 0 && ch <9) for(int i=0;i<ch;i++,read(STDIN_FILENO, &ch, 1));
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
    for(int i = 0 ; i < r_state->no_of_char ; i++){
        printf("%c", r_state->line[i]) ;
        fflush(stdout) ;
      }
  }

void insertline(){


      }

bool refresh_screen(editorState State, row_state* r_state, int bound ){
    printf("\e[1;1H") ;
  fflush(stdout) ;
  for(int i = 0 ; i < bound ; i++){
        nprintf(r_state) ;
        printf("\e[%d;%dH", State.fileposition_y,State.fileposition_x) ;
      fflush(stdout);
      return 0 ;
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


// SPECIAL KEY CSI
bool save_buffer(editorState* State, row_state* r_state){

        char input;
      read(STDIN_FILENO, &input, 1);
      switch(input) {
          case '\e':
            escseq key = CSI_code();
            handle_CSI(State, key);
            break;
          case 127:
            break;
          case '\n':

                    break;
          default:
            put_in_line(State,r_state, input) ;    

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


// void disable_raw_mode(struct termios* original_termios) {
  //     tcsetattr(STDIN_FILENO, TCSAFLUSH, original_termios);
// }


void initEditor(editorState* State){
    State->fileposition_x = 0 ;
    State->fileposition_y = 0 ;
    State->no_of_lines = 1 ;;
    get_window (&State->screen_rows, &State->screen_cols) ;
    character_art(State->screen_rows, State->screen_cols) ;
    //load the file || create the file
  }


int main(int argc,char* argv[]){
    editorState State ;
    row_state *r_state = malloc(sizeof(row_state));
    bool changeflag = 0 ;
    enable_raw_mode() ;
    initEditor(&State) ;
    while(1){
          if(changeflag) 
          changeflag = refresh_screen(State ,r_state, State.no_of_lines) ;
          changeflag = save_buffer(&State,r_state) ;
      }
  return 0;
}
