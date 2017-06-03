#ifndef _ZHSH_IO_H_
#define _ZHSH_IO_H_

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "error.h"

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define ESCAPE_CHAR 0x1B
#define BACKSPACE_CHAR 0x08
#define DELETE_CHAR    0x7F

#define getch(c)   read(STDIN, c, 1)
#define putch(c)   write(STDOUT, &c, 1)
#define print(msg) write(STDOUT, msg, strlen(msg))

int zhputs(char* str);

#endif
