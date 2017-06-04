#include "token.h"

token_t* new_token_bw(char*                     string, 
                      size_t                    length, 
                      tkn_escape_level_t        escape_level, 
                      tkn_preprocessing_level_t preproccessing_level) {
    token_t* token = malloc(sizeof(token_t)); 

    token->len = length;
    token->str = string;

    token->type                 = TKN_TYPE_BARE_WORD;
    token->escape_level         = escape_level;
    token->preproccessing_level = preproccessing_level;

    return token;
}

token_t* new_token_bd(tkn_type_t type) {
    token_t* token = malloc(sizeof(token_t)); 

    token->len  = 0;
    token->str  = NULL;
    token->type = type;

    token->escape_level         = TKN_ESCAPE_LEVEL_NO;
    token->preproccessing_level = TKN_PREPROCESSING_NO;

    return token;
}

void delete_token(void* data) {
    token_t* token = (token_t*) data;

    if (token->type == TKN_TYPE_BARE_WORD) {
        free(token->str);
    }

    free(token);
}
