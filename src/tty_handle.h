#ifndef _ZHSH_TTY_HANDLE_H_
#define _ZHSH_TTY_HANDLE_H_

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include "error.h"

struct termios saved_tty;
struct termios tty;

int set_tty_ncanon(void);
int reset_tty(void);

#endif
