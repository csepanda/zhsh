#ifndef _ZHSH_HISTORY_H
#define _ZHSH_HISTORY_H

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "../error.h"
#include "../tty_handle.h"
#include "../tty_navigation.h"
#include "../arraylist.h"
#include "../env.h"


void history_ini(char* buffer_ptr, int* buffer_index_ptr, 
                 tty_navigation_position_t* promt_position);
void history_save();
void history_reset();

void history_add_entry(char* line);
void history_down();
void history_up  ();

#endif

