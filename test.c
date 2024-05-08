#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <math.h>



typedef enum{
    OTHER,
    UP_ARROW,
    DOWN_ARROW,
    RIGHT_ARROW,
    LEFT_ARROW,
} special_code ;

int  read_special_code(){
    char code1 ;
    char code2 ;
    char code3 ;
    special_code  special_code ;
    read(STDIN_FILENO, &code1, 1);
    printf("%c",code1) ;
    if(code1 >= 0 && code1 < 10){
        read(STDIN_FILENO, &code2, 1);
        printf("%c", code2) ;   
    }
    read(STDIN_FILENO, &code3, 1) ;
    printf("%c", code3) ;        
    switch(code3){
        case 'a' :
            special_code = UP_ARROW ;
            break ;
        case 'b' :
            special_code = DOWN_ARROW ;
            break ;
        case 'c' :
            special_code = RIGHT_ARROW ;
            break ;
        case 'd' :
            special_code = LEFT_ARROW ; 
            break ;
    }
    return special_code ;
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

void move_cursor_back(int rows, int cols) {
    printf("\033[%dA\033[%dD", rows, cols); 
}


typedef enum {
    int mode_t;
    
} state;


int max(int a, int b) {
    return (a > b) ? a : b;
}

void render_footer() {
    
}

int main() {
    struct termios original_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    enable_raw_mode();

    render_footer();
    for(int i=0; i<40; i++) {
        for(int j=0; j<129; j++) {
            printf(".");
        }
    }
    fflush(stdout);

    char** g = malloc(sizeof(char*)*10000);
    int line_real;
    int colm_real;
    char c;
    int i = 0 ;
    while(true) {
        read(STDIN_FILENO, &c, 1); 
        printf("%c", c);
        fflush(stdout);
    
        if(c == 27) {
            switch (read_speacial_code()) {
                case UP_ARROW:
                    line_real = max(0, line_real-1);
                    break; 
                case DOWN_ARROW:
                    line_real = line_real;
                    break;
                case RIGHT_ARROW:
                    colm_real = colm_real+1; 
                    break;
                case LEFT_ARROW:
                    colm_real = max(0, colm_real-1);
                    break;
            }
            continue;
        }

        // Check if text already existed at this point
        if(g[line_real][colm_real] == 0) {
            
        }
    }
    disable_raw_mode(&original_termios);

    return 0;
}
    // printf("\033[6n"); // Request cursor position
    // int row, col;
    // scanf("\033[%d;%dR", &row, &col);
    // printf("GOT %d %d\n", row, col);
    // writeline() { putcursorlinstart; printf(line); flush; }
    // else if(c is ascii) { (*g)[f] = c; writeline((*g)) }
