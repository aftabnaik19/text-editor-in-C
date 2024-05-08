#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>

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

typedef enum  {
    UP_ARROW,
    DOWN_ARROW,
    RIGHT_ARROW,
    LEFT_ARROW,
} special_code;

// special_code read_special_code() {
    
// }

int main() {
    struct termios original_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    enable_raw_mode();

    printf("Hello, world!\n");

    for(int i=0; i<40; i++) {
        for(int j=0; j<129; j++) {
            printf(".");
        }
    }
    fflush(stdout);

    char** g = malloc(sizeof(char*)*10000);
    int f;
    char c;
    while(true) {
        read(STDIN_FILENO, &c, 1); 
        printf("%c", c);

        if(c == '\0') {
            read_speacial_code();
        }
        
        // if(c == "upprow") { g--; };
        // if(c == "right") { f++; };
        // writeline() { putcursorlinstart; printf(line); flush; }
        // else if(c is ascii) { (*g)[f] = c; writeline((*g)) }
        
        fflush(stdout);
    }

    fflush(stdout);
    disable_raw_mode(&original_termios);

    return 0;
}
    // printf("\033[6n"); // Request cursor position
    // int row, col;
    // scanf("\033[%d;%dR", &row, &col);
    // printf("GOT %d %d\n", row, col);
