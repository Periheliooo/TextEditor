/*** includes ***/

# include <ctype.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <termios.h>
# include <unistd.h>

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)     // write the terminal
        die("tcsetattr");    
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)     // read the terminal
        die("tcgetattr");    
    atexit(disableRawMode);     // register function to be called automatically
                                // when the program exit.
    
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);    // input flag
    raw.c_oflag &= ~(OPOST);    // 
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);     // input falg, not ECHO, turn off line buffering
                                                        // 
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");    
}

/*** init ***/

int main() {
    enableRawMode();

    char c;
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {    // test whether is a control character(ASCII 0-31 & 127)
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);    // %d decimal number(ASCII) %c direct
                                                // \r at the left side
        }
        if (c == 'q') break;
    }
    return 0;
}