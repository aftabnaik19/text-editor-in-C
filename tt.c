// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <termios.h>
// #include <sys/ioctl.h>
// #include <stdbool.h>
// #include <math.h>

#include <stdio.h>

typedef enum{
    OTHER,
    UP_ARROW,
    DOWN_ARROW,
    RIGHT_ARROW,
    LEFT_ARROW,
} SpecialCode ;

char read_special_code(){
    char code3 ;
    read(STDIN_FILENO, &code3, 1);
    // printf("%c",code3) ;
    read(STDIN_FILENO, &code3, 1) ;
    // printf("%c", code3) ;        
    // fflush(stdout);
    switch(code3){
        case 'A' : return 'u';
        case 'B' : return 'd';
        case 'C' : return 'r';
        case 'D' : return 'l';
    }
    return 'f';
}

// void enable_raw_mode() {
//     struct termios raw;
//     tcgetattr(STDIN_FILENO, &raw);
//     raw.c_lflag &= ~(ECHO | ICANON);
//     tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
// }

// void disable_raw_mode(struct termios* original_termios) {
//     tcsetattr(STDIN_FILENO, TCSAFLUSH, original_termios);
// }

void move_cursor_back(int rows, int cols) {
    printf("\033[%dA\033[%dD", rows, cols); 
}


int max(int a, int b) {
    return (a > b) ? a : b;
}

void render_footer() {
    
}

int main() {
    // struct termios original_termios;
    // tcgetattr(STDIN_FILENO, &original_termios);
    // enable_raw_mode();

    printf("\033[1;1H"); 

    // struct winsize size;
    // if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1) {
    //  perror("ioctl");
    //  return 1;
    // }

    int terminal_width = 100;//size.ws_col;
    int terminal_height= 100;//size.ws_row;

    
    // render_footer();
    char** g = malloc(sizeof(char*)*10000);
    g[0] = calloc(100, sizeof(char));
    int lines = 0;
    
    for(int i=0; i<terminal_height; i++) {
        for(int j=0; j<terminal_width; j++)
            printf(" ");
        printf("\n");
    }
    printf("\033[1;1H"); 
    printf("\033[47m \033[30;47m");
    printf("Spring Syntax");
    for(int j=0; j<terminal_width-14; j++) printf(" ");
    printf("\033[1;1H\033[1B\033[1B\033[0m");
    fflush(stdout);
    int line_real=0;
    int colm_real=0;
    char c;
    int i = 0 ;
    int count = 0 ;
    int countlen = 0 ;
    while(true) {
        // read(STDIN_FILENO, &c, 1); 
        c = getch();   
        fflush(stdout);
        if(c == 27) {
            char h = read_special_code();
            char f;
            switch (h) {
                case 'u':
                    line_real = max(0, line_real-1);
                    printf("\e[A");
                    fflush(stdout);
                    break;
                case 'd':
                    line_real = line_real+1;
                    if(line_real > lines) {
                        lines = max(line_real, lines);
                        g[line_real] = calloc(100, sizeof(char));
                    }
                    printf("\e[B");
                    fflush(stdout);
                    break;
                case 'r':
                    if(g[line_real][colm_real] != 0){
                        colm_real = colm_real+1;
                    printf("\e[C");
                    fflush(stdout);}
                    break; 
                case 'l':
                    colm_real = max(0, colm_real-1);
                    printf("\e[D");
                    fflush(stdout);
                    break;
            }
            continue;
        }
        printf("%c", c);
        fflush(stdout);

        // // Check if text already existed at this point
        // if(g[line_real][colm_real] == '\n') {
        //     char* nl = calloc(sizeof(char)*100);
        //     int len = strlen(g[line_real]);
        //     len--;
        //     strcpy(&g[line_real][colm_real], nl);
        //     memset(&g[line_real][colm_real], 0);
        //     while() { g[line_real][len] = g[]); }
        // }
        if(c == 'u') {
            break;
        }

        if(c == '\n' && g[line_real][colm_real] == 0) {
            colm_real=0;
            line_real++;
            g[line_real] = calloc(100, sizeof(char));
            lines = max(line_real, lines);
            continue;
        }
        if(c == '\n' && g[line_real][colm_real] != 0) {
            continue;
        }
        if( c == 127) {
            colm_real= max(0, colm_real-1);
            for(int i=colm_real; i<100; i++) {
                if(g[line_real][i] == 0) break;
                g[line_real][i] = (g[line_real][i+1]==0)?' ':g[line_real][i+1];
            }
            printf("\033[G");
            printf("%s", g[line_real]);
            printf("\033[G");
            printf("\033[%dC", colm_real);
            fflush(stdout);
           
            continue;
        }
        if(g[line_real][colm_real] != 0) {
            for(int i=79; i>=colm_real+1; i--) {
                g[line_real][i] = g[line_real][i-1];
            }
            g[line_real][colm_real] = c;
            printf("\033[G");
            printf("%s", g[line_real]);
            printf("\033[G");
            printf("\033[%dC", colm_real+1);
            fflush(stdout);
            
            colm_real++;
            continue;
        }

        if((c >= 'a' && c <= 'z') || c == ' ') { 
            g[line_real][colm_real] = c;
            colm_real++;
        } if(c >= 'A' && c <= 'Z') { 
            g[line_real][colm_real] = c;
            colm_real++;
        }
    }

    printf("line %d cols %d\n", lines, lines);
    for(int i=0; i<lines+1; i++) {
        for(int j=0; j<80; j++)
            printf("%c", g[i][j]);
        printf("\n");
    }
    return 0;
}
    // printf("\033[6n"); // Request cursor position
    // int row, col;
    // scanf("\033[%d;%dR", &row, &col);
    // printf("GOT %d %d\n", row, col);
    // writeline() { putcursorlinstart; printf(line); flush; }
    // else if(c is ascii) { (*g)[f] = c; writeline((*g)) }
