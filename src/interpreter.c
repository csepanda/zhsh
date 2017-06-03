#include "interpreter.h"

#define NO    0
#define YES   1

#define ESCAPE_LEVEL_NOESC         0
#define ESCAPE_LEVEL_QUOTES        1
#define ESCAPE_LEVEL_DOUBLE_QUOTES 2

#define STRHNDL_INITIAL_CAP 4096
#define STRHNDL_AUGMENT      255

#define TOKEN_PROCESSING_NO   0
#define TOKEN_PROCESSING_VAR  1
#define TOKEN_PROCESSING_GLOB 2
#define TOKEN_PROCESSING_BOTH 3


static int is_digit (char c) { 
    return (c >= '0' && c <= '9');
}

static int is_letter(char c) { 
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int is_vname_char(char c) {
    return is_digit(c) || is_letter(c) || c == '_';
}

static int is_path(char* s, size_t len) {
    return (len > 2 && s[0] == '.' && s[1] == '/') || s[0] == '/' ||
           (len > 3 && s[0] == '.' && s[1] == '.'  && s[2] == '/');
}

static int last_slash(char* str, int len) {
    int i; for (i = len; i >= 0; i--) {
        if (str[i] == '/') return i;
    }

    return -1;
}

static int regex_size(char* str, size_t len) {
    size_t i, c;
    for (i = 0, c = len; i < len; i++) {
        if (str[i] == '*' || str[i] == '[' || str[i] == ']' ||
            str[i] == '^' || str[i] == '$') { 
            c++;
        }
    }

    return c;
}

static int contains(const char* string, const regex_t* pattern) {
    return regexec(pattern, string, 0, NULL, 0) != REG_NOMATCH;
}

static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static void new_token(arraylist_t* tokens, int   proc, int    elvl, 
                                           char* str,  size_t len) {
    token_t* tkn = malloc(sizeof(token_t)); 
    tkn->elvl = elvl;
    tkn->proc = proc;
    tkn->len  = len;
    tkn->str  = str;
    persist_to_arraylist(tokens, tkn);
}

void delete_token(void* data) {
    token_t* token = (token_t*) data;
    free(token->str);
    free(token);
}

static arraylist_t* tokenize(char* line, const size_t length) {
    int    IS_TOKEN = NO;
    int    esc_lvl  = 0, tkn_proc = 0;
    char   token[STRHNDL_INITIAL_CAP];
    size_t i, j; char c;
    arraylist_t* tokens = new_arraylist(5 + length / 5, &delete_token);

    for (i = 0, j = 0, c = line[0]; i < length; c = line[++i]) {
        if (IS_TOKEN) {
            if ((esc_lvl == ESCAPE_LEVEL_NOESC                              && 
                      (i == length - 1 || is_whitespace(c)))                ||
                (esc_lvl == ESCAPE_LEVEL_QUOTES        && c == '\''       ) ||
                (esc_lvl == ESCAPE_LEVEL_DOUBLE_QUOTES && c == '\"'       )) {
                token[j] = '\0';
                new_token(tokens, tkn_proc, esc_lvl, strdup(token), j);

                esc_lvl  = 0;
                tkn_proc = 0;
                j        = 0;
            } else {
                if (esc_lvl != ESCAPE_LEVEL_QUOTES && 
                    c == '$' && token[j - 1] != '\\') {
                    tkn_proc |= TOKEN_PROCESSING_VAR;
                } else if (esc_lvl == ESCAPE_LEVEL_NOESC &&
                    (c == '*' || c == '?') && token[j - 1] != '\\') {
                    tkn_proc |= TOKEN_PROCESSING_GLOB;
                }
                token[j++] = c;
            }
        } else {
            if (is_whitespace(c)) { 
                continue;
            } else if (c == '\'') {
                esc_lvl = ESCAPE_LEVEL_QUOTES;
            } else if (c == '\"') {
                esc_lvl = ESCAPE_LEVEL_DOUBLE_QUOTES;
            } else {
                esc_lvl = ESCAPE_LEVEL_NOESC;
                if (c == '$') {
                    tkn_proc |= TOKEN_PROCESSING_VAR;
                } else if (c == '*' || c == '?') {
                    tkn_proc |= TOKEN_PROCESSING_GLOB;
                }
                token[j++] = c;
            }

            IS_TOKEN = YES;
        }
    }
    
    return tokens;
}

char* expand_variables(token_t* token) {
    int    POS_ARG = NO;
    int    ESCAPE  = NO;
    char   var_name[256];
    char*  str = token->str;
    char** to_merge;
    size_t i, j, bindex = 0; 
    size_t len = token->len;
    char   buffer[STRHNDL_INITIAL_CAP];

    arraylist_str_t* sep = new_arraylist_str(5 + token->len / 5);

    for (i = 0; i < len; i++) {
        if (str[i] == '$' && ESCAPE == NO) {
            char c = str[++i]; char* val; j = 0;

            if (bindex != 0) {
                buffer[bindex] = '\0'; bindex = 0;
                merge_to_arraylist_str(sep, buffer);
            }

            while (j < 256 && i < len && 
                ((POS_ARG == YES && is_digit(c)) ||
                 (POS_ARG ==  NO && is_vname_char(c)))) {
                var_name[j++] = c; c = str[++i];
            }
            var_name[j] = '\0';
            val = get_env(var_name);
            if (val != NULL) {
                merge_to_arraylist_str(sep, val);
            }
            i--;
        } else if (str[i] == '\\' && ESCAPE == NO) {
            ESCAPE = YES;
        } else {
            buffer[bindex++] = str[i]; ESCAPE = NO;
        }
    }

    if (bindex != 0) {
        buffer[bindex] = '\0';
        merge_to_arraylist_str(sep, buffer);
    }

    to_merge = to_array_str(sep);
    token->str = merge_strings(to_merge, '\0', NO); 
    token->len = strlen(token->str);

    free(str); for (i = 0; i < sep->size; i++) {
        free(to_merge[i]);
    }
    free(to_merge);
    remove_arraylist_str(sep);

    return token->str;
}

static int open_glob(arraylist_t* list, size_t index) {
    token_t* token = list->data[index];
    arraylist_t* expanded = new_arraylist(10, &delete_token);
    char c;
    char* str = token->str;
    char* path;
    char* pattern; regex_t regex;
    char  CUSTOM_PATH = YES;
    struct dirent* ent; DIR* dir;
    size_t i, j, plen;
    size_t len = token->len;
    path = strdup(token->str); i = last_slash(path, len);
    if (is_path(path, len)) {
        path[i + 1] = '\0';
    } else {
        CUSTOM_PATH = NO;
        free(path);
        path = ".";
    }

    if ((dir = opendir(path)) == NULL) {
        if (CUSTOM_PATH) { free(path); }
        free(expanded->data);
        free(expanded);
        return 0;
    }

    plen = regex_size(str + i + 1, len - i) + 2;
    pattern = malloc(sizeof(char)*(plen + 1));
    for (i++, j = 1, c = str[i]; i < len; c = str[++i]) {
        if (c == '*') {
            pattern[j++] = '.';
            pattern[j++] = '*';
        } else if (c == '?') {
            pattern[j++] = '.';
        } else if (c == '[' || c  == ']' || /* ignore special    */
                   c == '^' || c  == '$' || c == '.') { /* regex characters  */
            pattern[j++] = '\\';
            pattern[j++] = c;
        } else {
            pattern[j++] = c;
        }
    }
    pattern[0]   = '^';  /* must match */
    pattern[j++] = '$';  /* whole name */
    pattern[j]   = '\0';
    regcomp(&regex, pattern, 0);


    while ((ent = readdir(dir)) != NULL) {
        char* name = ent->d_name;
        if (pattern[1] != '\\' && pattern[2] != '.' && name[0] == '.') {
            continue;
        }

        if (contains(name, &regex)) { 
            new_token(expanded, TOKEN_PROCESSING_NO, ESCAPE_LEVEL_QUOTES, 
                      strdup(name), strlen(name));
        }
    }

    closedir(dir);
    regfree(&regex);
    
    if (CUSTOM_PATH) { free(path); }
    free(pattern);

    if (expanded->size != 0) {
        list->size--;
        merge_arraylists(list, expanded, index);
        delete_token(token);
        free(expanded->data);
        free(expanded);
        return 0;
    }

    free(expanded->data);
    free(expanded);

    return -1;
}

void handle_line(char* line, const size_t size) {
    char* IFS = get_env("IFS");
    char* token;
    char** to_merge;
    char** args;
    size_t i;
    arraylist_t* tokens = tokenize(line, size);
    arraylist_str_t* args_list;
    for (i = 0; i < tokens->size; i++) {
        token_t* t = tokens->data[i];
        if (t->proc & TOKEN_PROCESSING_VAR) {
            expand_variables(t);
        }
        if (t->proc & TOKEN_PROCESSING_GLOB) {
            open_glob(tokens, i);
        }
    }
    to_merge = malloc(sizeof(char*)*(tokens->size + 1));
    for (i = 0; i < tokens->size; i++) {
        to_merge[i] = ((token_t*)tokens->data[i])->str;
    }
    to_merge[i] = NULL;
    line = merge_strings(to_merge, ' ', NO);

    token     = strtok(line, IFS);
    args_list = new_arraylist_str(2 + tokens->size);
    while (token != NULL) {
        merge_to_arraylist_str(args_list, token);
        token     = strtok(NULL, IFS);
    } 


    if (args_list->size > 0) {
        size_t argc = args_list->size;
        args = to_array_str(args_list);
        if (exec_builtin_cmd(argc, args) != -2) {
            size_t i; for (i = 0; i < argc; i++) {
                free(args[i]);
            }
            free(args);
        } else {
            pid_t child = fork();
            if (child == -1) {
                alarm_msg(ALARM_CANNOT_EXEC);
            } else if (child > 0) {
                int num = add_job(child, argc, args);
                set_foreground_by_num(num);
            } else if (is_path(args[0], strlen(args[0]))) {
                reset_tty();
                execv(args[0], args);
                switch (errno) {
                case  ENOENT:
                case ENOTDIR:
                    send_errmsg(args[0], ALARM_FILE_NOT_FOUND);
                    break;
                default:
                    send_errmsg("", ALARM_RUNTIME_ERR);
                }
                _exit(127);
            } else {
                reset_tty();
                if (strchr(line, '&') != NULL) {
                }
                execvp(args[0], args);
                switch (errno) {
                case  ENOENT:
                case ENOTDIR:
                    send_errmsg(args[0], ALARM_CMD_NOT_FOUND);
                    break;
                default:
                    send_errmsg("", ALARM_RUNTIME_ERR);
                }
                _exit(127);
            }
        }
    }

    free(line);
    free(to_merge);
    remove_arraylist(tokens);
    remove_arraylist_str(args_list);
}

