#ifndef _ZHYSH_TTY_NAVIGATION_H
#define _ZHYSH_TTY_NAVIGATION_H

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "io.h"
#include "error.h"

#ifndef I_REWROTE
    #include <stdio.h>
#endif

#define TTY_MOVE_UP_STR     "[1A"
#define TTY_MOVE_DOWN_STR   "[1B"
#define TTY_MOVE_RIGHT_STR  "[1C"
#define TTY_MOVE_LEFT_STR   "[1D"

typedef enum tty_navigation_direction_t {
    TTY_MOVE_UP,
    TTY_MOVE_DOWN,
    TTY_MOVE_RIGHT,
    TTY_MOVE_LEFT
} tty_navigation_direction_t;

typedef struct _tty_navigation_position {
    uint32_t x;
    uint32_t y;
} tty_navigation_position_t;

typedef struct _tty_navigation {
    tty_navigation_position_t current_position;
    tty_navigation_position_t user_input_position;
} tty_navigation_t;

tty_navigation_t tty_navigation;

void tty_navigation_object_ini(void);
void tty_navigation_position_reset(void);
void tty_navigation_move(tty_navigation_direction_t direction);
void tty_navigation_move_to(tty_navigation_position_t position);
void tty_navigation_print_debug_info(void);

#endif
