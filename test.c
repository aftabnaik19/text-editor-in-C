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

void printCenteredString(int windowHeight, int windowWidth, const char *str) {
    int len = strlen(str);  // Calculate the length of the string
    // Print the left padding
    int leftPadding = (windowWidth - len) / 2;  // Calculate left padding
    leftPadding = (leftPadding < 0) ? 0 : leftPadding;
    for (int i = 0; i < leftPadding; i++) {
        printf(" ");
    }
    // Print the string
    printf("%s", str);
    // Move to the next line
    printf("\n");
}
void displayWelcomeScreen(int window_width , int window_height){
    //clears the whole screen
    printf("\e[2J");

    //set cursor position to top left
    printf("\e[H");

    // printf("\e[4m");     // Underline
    //sets the formatting for upcoming characters
    printf("\e[49;96m"); 
    printf("\e[1m");     // Bold
    printf("\e[6;%d;%dt", 20, 14);

    printf("\e[5m");    
    for(int i = 0 ; i < window_height/2 -10 ; i++){
        printf("\n");
    }                                                                                                                                          
                                                                                                                                                   
    printCenteredString(window_height,window_width, " #####   ###  ### ##   ##   # #####   ###    ###  ##            #####   ######    ##  ##  #######  ######   #######");
    printCenteredString(window_height,window_width, "##   ##   ##  ##  ###  ##  ## ## ##  ## ##   ###  ##           ##   ##   ##  ##   ##  ##   ##   #   ##  ##   ##   #");
    printCenteredString(window_height,window_width, "##         ####   #### ##     ##    ##   ##   #####            ##        ##  ##   ##  ##   ##       ##  ##   ##    ");
    printCenteredString(window_height,window_width, " #####      ##    #######     ##    ##   ##    ###              #####    #####    ######   ####     #####    ####  ");
    printCenteredString(window_height,window_width, "     ##     ##    ## ####     ##    #######   #####                 ##   ##       ##  ##   ##       ## ##    ##    ");
    printCenteredString(window_height,window_width, "##   ##     ##    ##  ###     ##    ##   ##  ##  ###           ##   ##   ##       ##  ##   ##   #   ## ##    ##   #");
    printCenteredString(window_height,window_width, " #####     ####   ##   ##    ####   ##   ##  ##  ###            #####   ####      ##  ##  #######  #### ##  #######");
                                                                                                                     
    printf("\n");
    printCenteredString(window_height,window_width,"SyntaxSphere\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printCenteredString(window_height,window_width," Use arrow keys to move around, and press Ctrl+Q to quit.\n");
    printCenteredString(window_height,window_width," Use arrow keys to move around, and press Ctrl+Q to quit.\n");
    printCenteredString(window_height,window_width," Use arrow keys to move around, and press Ctrl+Q to quit.\n");
    printCenteredString(window_height,window_width," Use arrow keys to move around, and press Ctrl+Q to quit.\n");
    printCenteredString(window_height,window_width," Use arrow keys to move around, and press Ctrl+Q to quit.\n");
    saveToFile();
    // printf("\n");
    // printf("The window width is %d and window height is %d", window_width,window_height );
}

void saveToFile(){
    
    FILE* filePointer;
    //change total string to no_of_lines
    int total_strings = 50  , max_string_length=20;
    // Get the data to be written in file
    char data[50][20] ;
 
    // Open the existing file GfgTest.c using fopen()
    // in write mode using "w" attribute
    filePointer = fopen("output.txt", "w");
 
    // Check if this filePointer is null
    // which maybe if the file does not exist
    if (filePointer == NULL) {
        printf("output.txt file failed to open.");
    }
    else {

        for(int i = 0 ; i < 50 ; i++){
            for(int j = 0 ; j < 20 ; j++){
                data[i][j] = (char)('A'+i+j);
            }
            // if (strlen(data[i]) > 0) {
            for(int j = 0 ; j < 20 ; j++){
                if(data[i][j]=='\n')break;
                fprintf(filePointer,"%c", data[i][j] );
            }
    
                // writing in the file using fputs()
                // fputs(data[i], filePointer);
                fputs("\n", filePointer);
             
        }
 
        printf("The file is now opened.\n");
 
        // Write the dataToBeWritten into the file
 
        // Closing the file using fclose()
        fclose(filePointer);
 
        printf("Data successfully written in file ");
    }
}

int main() {
    struct termios original_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    enable_raw_mode();
    // Get terminal size
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1) {
        perror("ioctl");
        return 1;
    }

    printf("Terminal width: %d\n", size.ws_col);
    printf("Terminal height: %d\n", size.ws_row);

    displayWelcomeScreen(size.ws_col,size.ws_row);
    fflush(stdout);

    char** g = malloc(sizeof(char*)*10000);
    int f;
    char c;
    while(true) {
        read(STDIN_FILENO, &c, 1); 
        printf("%c", c);

        if(c == '\0') {
            // read_special_code();
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
