#include "preprocessor.h"

#define NO    0
#define YES   1
#define STRHNDL_INITIAL_CAP 4096
#define STRHNDL_AUGMENT      255

char* expand_variables_str(char* str) {
    token_t token;
    token.str = strdup(str);
    token.len = strlen(str);
    return expand_variables(&token);
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

int open_glob(arraylist_t* list, size_t index) {
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
            char* str; size_t len; token_t* tkn;
            const tkn_preprocessing_level_t plvl = TKN_PREPROCESSING_NO;
            const tkn_escape_level_t        elvl = TKN_ESCAPE_LEVEL_NO;

            if (CUSTOM_PATH) {
                str = calloc(strlen(name) + strlen(path) + 1, sizeof(char));
                strcat(str, path);
                strcat(str, name);
                len = strlen(str);
            } else {
                str = strdup(name);
                len = strlen(name);
            }

            tkn = new_token_bw(str, len, elvl, plvl);
            persist_to_arraylist(expanded, tkn);
        }
    }

    closedir(dir);
    regfree(&regex);
    
    if (CUSTOM_PATH) { free(path); }
    free(pattern);

    if (expanded->size != 0) {
        size_t count = expanded->size - 1;
        remove_from_arraylist(list, index);
        merge_arraylists(list, expanded, index);
        free(expanded->data);
        free(expanded);
        return count;
    }

    free(expanded->data);
    free(expanded);

    return 0;
}
