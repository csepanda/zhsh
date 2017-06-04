#ifndef _ZHSH_PARSER_TOKEN_H
#define _ZHSH_PARSER_TOKEN_H

#include <stdlib.h>

#define ESCAPE_LEVEL_NOESC         0
#define ESCAPE_LEVEL_QUOTES        1
#define ESCAPE_LEVEL_DOUBLE_QUOTES 2

#define STRHNDL_INITIAL_CAP 4096
#define STRHNDL_AUGMENT      255

#define TOKEN_PROCESSING_NO   0
#define TOKEN_PROCESSING_VAR  1
#define TOKEN_PROCESSING_GLOB 2
#define TOKEN_PROCESSING_BOTH 3

typedef enum { 
    TKN_PREPROCESSING_NO,   TKN_PREPROCESSING_VAR, 
    TKN_PREPROCESSING_GLOB, TKN_PREPROCESSING_BOTH 
} tkn_preprocessing_level_t;

typedef enum {
    TKN_ESCAPE_LEVEL_NO,
    TKN_ESCAPE_LEVEL_QUOTES,
    TKN_ESCAPE_LEVEL_DOUBLE_QUEOTES
} tkn_escape_level_t;

typedef enum {
    TKN_TYPE_BARE_WORD,
    TKN_TYPE_BIND_SEMICOLON,
    TKN_TYPE_BIND_AND,
    TKN_TYPE_BIND_OR,
    TKN_TYPE_BIND_PIPE,
    TKN_TYPE_BIND_BG,

    TKN_TYPE_IN_REDIR,
    TKN_TYPE_OUT_REDIR_REWRITE,
    TKN_TYPE_OUT_REDIR_APPEND
} tkn_type_t;

typedef struct {
    char*  str;
    size_t len;

    tkn_type_t                type;
    tkn_escape_level_t        escape_level;
    tkn_preprocessing_level_t preproccessing_level;
} token_t;

token_t* new_token_bw(char*                     string, 
                      size_t                    length, 
                      tkn_escape_level_t        escape_level, 
                      tkn_preprocessing_level_t preproccessing_level);
token_t* new_token_bd(tkn_type_t type);
void     delete_token(void* token);



#endif
