#ifndef _ZHSH_AUTOCOMPLETE_H_
#define _ZHSH_AUTOCOMPLETE_H_

#define  _POSIX_SOURCES 1
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include "../parser/recognizer.h"
#include "../arraylist.h"
#include "../tty_navigation.h"
#include "../tty_handle.h"
#include "../env.h"

typedef enum { 
    COMPL_TYPE_FILE, 
    COMPL_TYPE_COMMAND, 
    COMPL_TYPE_OPT 
} compl_type_t;

void completion_init(char* buffer_ptr, int* buffer_ind_ptr);
int  complete(void);

#endif

