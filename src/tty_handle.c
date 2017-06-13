#include "tty_handle.h"

struct termios saved_tty;
struct termios nocan_tty;
struct termios tty;

int set_tty_ncanon(void) {
    if (!isatty(0)) {
        panic(PANIC_STDIN_NOTTY, PANIC_ZHSH_INITIALIZATION_ERROR_CODE);
    }

    tcgetattr(0, &tty);
    saved_tty = tty;
    tty.c_lflag &= ~(ICANON|ECHO);
    tty.c_cc[VMIN] = 1;
    tcsetattr(0, TCSAFLUSH, &tty);
    tcgetattr(0, &nocan_tty);
    return 0;
}

int setup_tty(void) {
    tcsetattr(0, TCSAFLUSH, &nocan_tty);
    return 0;
}

int reset_tty(void) {
    tcsetattr(0, TCSADRAIN, &saved_tty);
    return 0;
}
