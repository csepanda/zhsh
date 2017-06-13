#ifndef _ZHSH_PARSER_RECOGNIZER_H_
#define _ZHSH_PARSER_RECOGNIZER_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <regex.h>
#include "token.h"

int is_digit     (char c);
int is_letter    (char c);
int is_vname_char(char c); /* valid variable name char */
int is_path      (char* s, size_t len);
int is_whitespace(char c);
int last_slash   (char* str, size_t len);
int digitful     (char* str, size_t len);
int is_empty     (char* str, size_t len);
int regex_size   (char* str, size_t len);
int contains     (const char* string, const regex_t* pattern);

int is_io_redirection_token(token_t* token);

#endif

