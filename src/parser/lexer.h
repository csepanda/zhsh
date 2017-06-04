#ifndef _ZHSH_PARSER_LEXER_H_
#define _ZHSH_PARSER_LEXER_H_

#include <stdarg.h>
#include <string.h>
#include <regex.h>

#include "recognizer.h"
#include "token.h"
#include "../util.h"
#include "../arraylist.h"

arraylist_t* tokenize(char* line, const size_t len);

#endif
