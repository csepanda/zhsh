#ifndef _ZHSH_INTERPRETER_H_
#define _ZHSH_INTERPRETER_H_

#define _POSIX_SOURCE 1
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <regex.h>
#undef  _POSIX_SOURCE
#include "../tty_handle.h"
#include "../util.h"
#include "../jobs.h"
#include "../builtin.h"
#include "../env.h"
#include "../arraylist.h"
#include "../stringbuilder.h"

void  handle_line(char* line, const size_t size);

#endif

