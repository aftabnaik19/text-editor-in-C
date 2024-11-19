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
#include <sys/wait.h>
 
#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
 
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
  int display_start_line,display_end_line ;
  int screen_rows ;
  int screen_cols ;
  int no_of_lines  ; 
  int no_of_rows  ;
  int *index_table ;
  char mode ;
  char* filename ;
  FILE *fp ; 
} editorState ;
 
 
typedef struct buffer{
  row_state rState ;
  editorState eState ;
}buffer ;
 
typedef struct stack{
  buffer* buffer ;
  size_t top ;
  int capacity ;
}stack ;
 
stack s ;
 
row_state create_row(row_state rs){
 row_state r = rs;
 r.line = (char*)malloc(rs.no_of_char) ;
 
 for(int i = 0 ; i < rs.no_of_char ; i ++ ){
   r.line[i] = rs.line[i]; 
 }
 return r ;
}
 
editorState create_editorState(editorState es){
  editorState e  = es;
  e.mode = '\e';
  e.index_table = malloc(es.no_of_lines * sizeof(int)); 
  for(int i = 0 ; i < es.no_of_lines ; i++){
    e.index_table[i] = es.index_table[i] ;
  }
  return e ;
}
 
buffer create_buffer(editorState es, row_state rs){
  buffer b;
  b.eState = create_editorState(es) ;
  b.rState = create_row(rs) ;
  return b ;
}
 
void create_stack(){
  s.buffer = malloc(sizeof(buffer)) ;
  s.capacity = 1 ;
  s.top = 0;
}
 
void add_to_stack(editorState e, row_state r){
  //DO: add element to stack
  s.buffer[s.top] = create_buffer(e, r) ;
  s.buffer = realloc(s.buffer, (s.top + 14)*sizeof(buffer)) ;
  s.top++ ;
}
 
buffer undo(){
  s.top--;  
  return s.buffer[s.top] ;
}
 
buffer redo(){
  s.top++ ;
  return s.buffer[s.top] ;
}
 
void render_headder(editorState State) {
    printf("\033[1;1H"); 
    printf("\033[47m \033[30;47m");
    printf(" Noter ");
    for(int j=0; j<State.screen_cols-7; j++) printf(" ");
    printf("\033[0m");
    fflush(stdout);
}
 
void render_footer(editorState State) {
    printf("\033[%d;%dH", State.screen_rows-1, 1); 
    printf("\033[47m \033[30;47m");
    if(State.mode == 'i') printf(" INS ") ;
    else if(State.mode == '\e') printf(" NOR ") ;
    printf("| line %d col %d", State.fileposition_y, State.fileposition_x);
    printf("     | %d",(int)s.top);
    for(int j=0; j<State.screen_cols-22; j++) printf(" ");
    printf("\033[1;1H\033[0m");
    fflush(stdout);
}
 
void line_alloc(row_state *r_state, int no_char){
  r_state->line = realloc(r_state->line,no_char) ;
}
 
row_state* get_row_at(editorState *State, row_state *r_state, int position) {
  return r_state + State->index_table[position];
} 
 
bool save_to_file(editorState *State, row_state *r_state){
   FILE *fp = fopen("code.c","w+") ;
 
   for(int i = 0 ; i < State->no_of_rows ; i++){
 
     char *line = get_row_at(State, r_state, i)->line;
     for(int j = 0 ; j < get_row_at(State, r_state, i)->no_of_char ; j++){
       fputc(line[j], fp);
     }
   }
   fclose(fp) ;
   exit(0) ; }
 
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
 
  // update the index table
  pos_y++;
  for(int i=State->no_of_rows; i> pos_y ; i--)
     State->index_table[i] = State->index_table[i-1];
  State->index_table[pos_y] = State->no_of_lines;
 
  // Update the state
  State->no_of_lines++;
  State->no_of_rows++ ;
  State->fileposition_x= 0;
  State->fileposition_y++;
}
 
void clear_display(){
  printf("\e[2J") ;
}
 
void cursor_to(int y,int x){
  printf("\e[%d;%dH", y, x);
}
 
escseq CSI_code(editorState *State, row_state *r_state){
  char ch ; 
  escseq escseq ;
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
        State->fileposition_y = max(State->fileposition_y - 1, 0);
        State->fileposition_x = min(r_state[State->index_table[State->fileposition_y]].no_of_char, State->fileposition_x);
      break;
    case DOWN:
        State->fileposition_y = min(State->fileposition_y + 1, State->no_of_rows - 1);
        State->fileposition_x = min(r_state[State->index_table[State->fileposition_y]].no_of_char, State->fileposition_x);
      break;
    case RIGHT:
      if(State->fileposition_x == r_state[State->index_table[State->fileposition_y]].no_of_char ){
        if(State->fileposition_y + 1 < State->no_of_rows){  
         State->fileposition_y++;
         State->fileposition_x = 0;
        }         
      } else { 
        State->fileposition_x++;
      }
      break;
 
    case LEFT:
      if(State->fileposition_x == 0) {              
         if(State->fileposition_y - 1 >= 0 ){  
           State->fileposition_y--;
           State->fileposition_x = r_state[State->index_table[State->fileposition_y]].no_of_char;
        }
      } else {  
        State->fileposition_x--;
      }
      break;
  }
}
 
void delete_line(editorState *State, int line_no){
  for(int i = 0 ; i < State->no_of_rows ; i++){
    if(i >= line_no){
      State->index_table[i] = State->index_table[i+1];
    }   
  }
  State->no_of_rows--;
}
 
void backSpace(editorState *State , row_state *r_state){
 
  printf("\e[2J") ;
  row_state *curr = get_row_at(State, r_state, State->fileposition_y);
  if(State->fileposition_x<=0)
  {
     row_state *last = get_row_at(State, r_state, State->fileposition_y-1);
     if(State->fileposition_y == 0) return ;
     if(State->fileposition_x < curr->no_of_char)
     {
       printf("hello");
       fflush(stdout) ;
       memcpy(last->line+last->no_of_char-1, curr->line, curr->no_of_char);
       last->no_of_char+= curr->no_of_char;
     }
     int posx = last->no_of_char - curr->no_of_char - 1 ;
     State->fileposition_y--;
     State->fileposition_x = posx;
     delete_line(State, State->fileposition_y+1) ;
  }
  else{
    for( int i = State->fileposition_x -1; i < curr->no_of_char ; i++)
      curr->line[i]=curr->line[i+1];
    curr->no_of_char--;
    State->fileposition_x--;
  }
}
 
bool save_buffer(editorState *State, row_state **r_state, char input){  
    switch(input) {
      case '\e':       
        State->mode = input ;
        break;
      case 127:
        backSpace(State ,*r_state);
        break;
      case '\n':
        newline(State, r_state);
        break;
      default: 
       for(int i = get_row_at(State, *r_state, State->fileposition_y)->no_of_char ; i >= State->fileposition_x + 1; i--){
         (*r_state)[State->index_table[State->fileposition_y]].line[i] = (*r_state)[State->index_table[State->fileposition_y]].line[i-1] ;
       }
       if( (get_row_at(State, *r_state, State->fileposition_y)->no_of_char + 1)%50 == 0)
         line_alloc(get_row_at(State, *r_state, State->fileposition_y), get_row_at(State, *r_state, State->fileposition_y)->no_of_char+50) ; 
       get_row_at(State, *r_state, State->fileposition_y)->line[State->fileposition_x] = input;
       State->fileposition_x++;
       get_row_at(State, *r_state, State->fileposition_y)->no_of_char++;
    }
    return 1 ;
}
 
void normal_mode(editorState *State, row_state **r_state, char input) {
    switch(input) {
        case 'i':
            State->mode = input;
            break;
        case 'h':
            if(State->fileposition_x == 0) {              
               if(State->fileposition_y - 1 >= 0 ){  
               State->fileposition_y--;
               State->fileposition_x = r_state[State->index_table[State->fileposition_y]]->no_of_char ;
               }
            } else {  
               State->fileposition_x--;
            }
            break;
        case 'j':
            State->fileposition_y = min(State->fileposition_y + 1, State->no_of_rows - 1);
            State->fileposition_x = min(r_state[State->index_table[State->fileposition_y]]->no_of_char , State->fileposition_x);
            break;
        case 'k':
            State->fileposition_y = max(State->fileposition_y - 1, 0);
            State->fileposition_x = min(r_state[State->index_table[State->fileposition_y]]->no_of_char , State->fileposition_x);
            break;
        case 'l':
            if(State->fileposition_x == r_state[State->index_table[State->fileposition_y]]->no_of_char){
               if(State->fileposition_y + 1 < State->no_of_rows){  
                  State->fileposition_y++;
                  State->fileposition_x = 0;
               }         
            } else { 
                  State->fileposition_x++;
            }
            break;
        case 'r':
            break;
        case 'd':
            delete_line(State, State->fileposition_y) ;
            break;
        case 'u':
          {
            // printf("exitint tere maa ki chu");
            // exit(0);
            buffer b = undo();
            *State = b.eState ;
            **r_state = b.rState ;
            break;
           }
            //DO: go down in stack ;
        case 'U':
            //DO: go up in stack ;
          {
            // exit(0);
            buffer b = redo();
            *State = b.eState ;
            **r_state = b.rState ;
            break;
          }
        default:
            break;
    }
}
 
void move_cursor_to_home() {
  printf("\e[2;1H") ;
}
 
void nprintf(row_state *r_state, editorState State,int line_no, int b){
  row_state *line = get_row_at(&State, r_state, line_no) ;
  move_cursor_to_home() ;
  cursor_to(line_no + 3 - b , 4);
  printf("\e[0K") ;
  printf("\e[1;0m%d\t", line_no+1);
  if(State.fileposition_y == line_no) printf("|");
  else printf(" ");
  printf(" \e[1;0m");
  int len = strlen(State.filename);
  for(int i = 0 ; i < line->no_of_char ; i++)
    printf("%c", line->line[i]) ;
  printf("\e[0K") ;
  printf(" \e[0m");
  fflush(stdout) ;
}
 
bool refresh_screen(editorState *State, row_state *r_state, int bound ){
 
  printf("\033[2J\033[H");
  render_footer(*State);
  render_headder(*State);
  printf("\e[?25l");
  int start = State->offset_y;
  if((State->fileposition_y - start) > State->screen_rows - 8  ) State->offset_y++ ;
  else if((State->fileposition_y - start) < 3 && State->fileposition_y > 2) State->offset_y-- ;
  start = State->offset_y;
  int end   = min(start + State->screen_rows, bound);
  end = min(start + State->screen_rows - 5, bound) ;
 
  for(int i = start; i < end ; i++){
    nprintf(r_state, *State, i, start) ;
  }
 
  cursor_to(State->fileposition_y+3 - start, State->fileposition_x+11) ;
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
  State->no_of_lines = 1 ;
  State->no_of_rows = 1;
  State->mode = '\e' ;
  get_window (&State->screen_rows, &State->screen_cols) ;
  clear_display();
  fflush(stdout);
  render_headder(*State);
}
 
void load_file(editorState *State, row_state **r_state, const char *filename) {
   // TODO : remove newlines from buffer 
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
 
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int row_count = 0;
 
    while ((read = getline(&line, &len, file)) != -1) {
        if (row_count >= State->no_of_lines) {
            *r_state = realloc(*r_state, sizeof(row_state) * (State->no_of_lines + 10));
            State->index_table = realloc(State->index_table, sizeof(int) * (State->no_of_lines + 10));
            State->no_of_lines += 10;
        }
        int p=1;
        row_state *new_row = *r_state + row_count;
        new_row->line = malloc((((len)/50)+1)*50);
        int j=0;
        for(int i=0; i<len-2; i++) {
          new_row->line[i-j] = line[i];
        }
        new_row->no_of_char = read;
        new_row->line_no = row_count + 1;
 
        State->index_table[row_count] = row_count;
        row_count++;
    }
 
    State->no_of_rows = row_count;
    State->no_of_lines = row_count;
 
    free(line);
    fclose(file);
}
 
void handle_input(editorState *State, row_state **r_state){
  char input;
  read(STDIN_FILENO, &input, 1);
  if(input == '\e'){
    char curr_mode = State->mode;
    State->mode = '\e';
    refresh_screen(State, *r_state, State->no_of_rows);
    read(STDIN_FILENO, &input, 1);
    if(input == '['){
          State->mode=curr_mode;
          escseq key = CSI_code(State, *r_state);
          handle_CSI(State, key, *r_state);
          return;     
     }
  }
   switch(State->mode){
    case 'i' :{  
        // //DO: save current buffer and add to stack 
        add_to_stack(*State, **r_state) ;
        save_buffer(State, r_state, input) ;//TODO : change save buffer to insertion mode
        break ;
     } 
    case '\e' :{
        normal_mode(State, r_state, input) ;
        break ;
     }
  }
}
 
int main(int argc, char *argv[]){
  editorState State ;
  row_state *r_state = malloc(sizeof(row_state) * 10);
  State.index_table  = malloc(sizeof(int) * 10);
  for(int i=0; i < 10; i++) State.index_table[i] = 0;
  r_state->line = malloc(sizeof(char)*51);
  r_state->line_no = 1;
  bool changeflag = 1 ;
  create_stack();
  add_to_stack(State, *r_state);
  initEditor(&State) ;
      if (argc > 1) {
            load_file(&State, &r_state, argv[1]);
            State.filename = argv[1];
      } else {
            State.filename = "untitled";  
      }
  enable_raw_mode() ;
  while(1){
    if(1)
      changeflag = refresh_screen(&State ,r_state, State.no_of_rows) ;
    handle_input(&State, &r_state) ;
  }
  return 0 ;
}
