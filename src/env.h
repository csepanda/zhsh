#ifndef _ZHSH_ENVVAR_H_
#define _ZHSH_ENVVAR_H_

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "hashtable.h"
#include "error.h"
#include "util.h"

void env_init();
void debug_env();

int   set_env  (char* key, char* name);
int   unset_env(char* key);
char* get_env  (char* key);

int shift_pos_args(size_t count);


#endif
