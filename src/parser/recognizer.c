#include "recognizer.h"

int is_digit (char c) { 
    return (c >= '0' && c <= '9');
}

int is_letter(char c) { 
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_vname_char(char c) {
    return is_digit(c) || is_letter(c) || c == '_';
}

int is_path(char* s, size_t len) {
    return (len > 2 && s[0] == '.' && s[1] == '/') || s[0] == '/' ||
           (len > 3 && s[0] == '.' && s[1] == '.'  && s[2] == '/');
}

int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

int is_empty(char* str, size_t len) {
    size_t i; for (i = 0; i < len; i++) {
        if (!is_whitespace(str[i])) {
            return 0;
        }
    }

    return 1;
}

int last_slash(char* str, size_t len) {
    int i; for (i = len; i >= 0; i--) {
        if (str[i] == '/') return i;
    }

    return -1;
}

int digitful(char* str, size_t len) {
    size_t i; for (i = 0; i < len; i++) {
        if (!is_digit(str[i])) {
            return 0;
        }
    }

    return 1;
}

int regex_size(char* str, size_t len) {
    size_t i, c;
    for (i = 0, c = len; i < len; i++) {
        if (str[i] == '*' || str[i] == '[' || str[i] == ']' ||
            str[i] == '^' || str[i] == '$') { 
            c++;
        }
    }

    return c;
}

int contains(const char* string, const regex_t* pattern) {
    return regexec(pattern, string, 0, NULL, 0) != REG_NOMATCH;
}

int is_io_redirection_token(token_t* token) {
    tkn_type_t type = token->type;
    return type == TKN_TYPE_IN_REDIR          || 
           type == TKN_TYPE_OUT_REDIR_REWRITE ||
           type == TKN_TYPE_OUT_REDIR_APPEND;
}

