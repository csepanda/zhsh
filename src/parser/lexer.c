#include "lexer.h"

#define NO    0
#define YES   1

#define STRHNDL_INITIAL_CAP 4096
#define STRHNDL_AUGMENT      255

static void reset(int* is_token, size_t* index,
                  tkn_escape_level_t*        esc_lvl, 
                  tkn_preprocessing_level_t* tkn_proc) {
    *is_token = NO;
    *index    = 0;
    *esc_lvl  = TKN_ESCAPE_LEVEL_NO;
    *tkn_proc = TKN_PREPROCESSING_NO;
}

static void push_word(arraylist_t* storage, char* token_buffer, size_t len, 
                      tkn_escape_level_t esc_lvl,
                      tkn_escape_level_t tkn_proc) {
    char* str; token_buffer[len] = '\0';
    str = strdup(token_buffer);
    persist_to_arraylist(storage, new_token_bw(str, len, esc_lvl, tkn_proc));
}

static void push_bind(arraylist_t* storage, tkn_type_t type) {
    persist_to_arraylist(storage, new_token_bd(type));
}

#define is_word_end (i == length - 1 || is_whitespace(c))
arraylist_t* tokenize(char* line, const size_t length) {
    int  IS_TOKEN = NO; size_t i, j; char c;
    char token[STRHNDL_INITIAL_CAP];
    tkn_escape_level_t        esc_lvl  = TKN_ESCAPE_LEVEL_NO;
    tkn_preprocessing_level_t tkn_proc = TKN_PREPROCESSING_NO;
    arraylist_t* tokens = new_arraylist(5 + length / 5, &delete_token);

    for (i = 0, j = 0, c = line[0]; i < length; c = line[++i]) {
        if (IS_TOKEN) {
            if ((esc_lvl == TKN_ESCAPE_LEVEL_NO && is_word_end            ) ||
                (esc_lvl == TKN_ESCAPE_LEVEL_QUOTES         && c == '\''  ) ||
                (esc_lvl == TKN_ESCAPE_LEVEL_DOUBLE_QUEOTES && c == '\"'  )) {
                push_word(tokens, token, j, esc_lvl, tkn_proc);
                reset(&IS_TOKEN, &j, &esc_lvl, &tkn_proc);
            } else {
               if (esc_lvl != ESCAPE_LEVEL_QUOTES && 
                    c == '$' && token[j - 1] != '\\') {
                    tkn_proc |= TOKEN_PROCESSING_VAR; 
                } else if (esc_lvl == ESCAPE_LEVEL_NOESC) {
                    if ((c == '*' || c == '?') && token[j - 1] != '\\') {
                        tkn_proc |= TOKEN_PROCESSING_GLOB;
                    } else if (c == ';' && token[j - 1] != '\\') {
                        push_word(tokens, token, j, esc_lvl, tkn_proc);
                        push_bind(tokens, TKN_TYPE_BIND_SEMICOLON);
                        reset(&IS_TOKEN, &j, &esc_lvl, &tkn_proc);
                    } else if (c == '|' && token[j - 1] != '\\') {
                        push_word(tokens, token, j, esc_lvl, tkn_proc);
                        reset(&IS_TOKEN, &j, &esc_lvl, &tkn_proc);
                        if (line[i + 1] == '|') {
                            push_bind(tokens, TKN_TYPE_BIND_OR); i++;
                        } else {
                            push_bind(tokens, TKN_TYPE_BIND_PIPE);
                        }
                    } else if (c == '&' && token[j - 1] != '\\') {
                        push_word(tokens, token, j, esc_lvl, tkn_proc);
                        reset(&IS_TOKEN, &j, &esc_lvl, &tkn_proc);
                        if (line[i + 1] == '&') {
                            push_bind(tokens, TKN_TYPE_BIND_AND); i++;
                        } else {
                            push_bind(tokens, TKN_TYPE_BIND_BG);
                        }
                    } else {
                        token[j++] = c;
                    }
                } else {
                    token[j++] = c;
                }
            }
        } else {
            if (is_whitespace(c)) continue;
            if (c == ';') {
                push_bind(tokens, TKN_TYPE_BIND_SEMICOLON);
            } else if (c == '|') {
                if (line[i + 1] == '|') {
                    push_bind(tokens, TKN_TYPE_BIND_OR); i++;
                } else {
                    push_bind(tokens, TKN_TYPE_BIND_PIPE);
                }
            } else if (c == '&') {
                if (line[i + 1] == '&') {
                    push_bind(tokens, TKN_TYPE_BIND_AND); i++;
                } else {
                    push_bind(tokens, TKN_TYPE_BIND_BG);
                }
            } else {
                if (c == '\'') { 
                    esc_lvl = TKN_ESCAPE_LEVEL_QUOTES;
                } else if (c == '\"') { 
                    esc_lvl = TKN_ESCAPE_LEVEL_DOUBLE_QUEOTES;
                } else {
                    esc_lvl = ESCAPE_LEVEL_NOESC;
                    if (c == '$') { 
                        tkn_proc |= TKN_PREPROCESSING_VAR;
                    } else if (c == '*' || c == '?') { 
                        tkn_proc |= TKN_PREPROCESSING_GLOB;
                    }
                    token[j++] = c;
                }
                IS_TOKEN = YES;
            }
        }
    }
    
    return tokens;
}
#undef is_end

