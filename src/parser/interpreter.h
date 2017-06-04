#ifndef _ZHSH_INTERPRETER_H_
#define _ZHSH_INTERPRETER_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "../tty_handle.h"
#include "../util.h"
#include "../jobs.h"
#include "../builtin.h"
#include "../env.h"
#include "../arraylist.h"

void  handle_line(char* line, const size_t size);

#endif

