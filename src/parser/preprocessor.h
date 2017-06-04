#ifndef _ZHSH_PARSER_PREPROCESSOR_H_
#define _ZHSH_PARSER_PREPROCESSOR_H_

#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "recognizer.h"
#include "token.h"
#include "../env.h"
#include "../arraylist.h"
#include "../util.h"

char* expand_variables(token_t* token);
char* expand_variables_str(char* str);
int   open_glob(arraylist_t* list, size_t index);
#endif
