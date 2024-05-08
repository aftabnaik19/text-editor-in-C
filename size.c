#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main() {
    struct winsize size;

    // Get terminal size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1) {
        perror("ioctl");
        return 1;
    }

    printf("Terminal width: %d\n", size.ws_col);
    printf("Terminal height: %d\n", size.ws_row);

    return 0;
}
