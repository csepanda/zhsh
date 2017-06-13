#ifndef _ZHSH_TTY_HANDLE_H_
#define _ZHSH_TTY_HANDLE_H_

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include "tty_navigation.h"
#include "error.h"

#define  clear_to_end write(1, "[0J", 4)

int set_tty_ncanon(void);
int setup_tty(void);
int reset_tty(void);

#endif
