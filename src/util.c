#include "util.h"

int str_is_num(char* str) {
    int i = 0; size_t len = strlen(str);
    if (str[0] == '-') {
        if (len < 2) {
            return 0;
        } else {
            i++;
        }
    }
    for (; i < len; i++) {
        char c = str[i];
        if (c < '0' || c > '9') {
            return 0;
        }
    }

    return 1;
}

uint32_t int_to_string(int32_t number, char buffer[]) {
    int length, shift = number;
    char const digit[] = "0123456789";
    char* p = buffer;

    if (number < 0) {
        *p++ = '-';
        number *= -1;
        shift   = number;
    }

    do { ++p; shift /= 10; } while (shift);
    *p = '\0';
    length = p - buffer;
    do { *--p = digit[number % 10]; number /= 10; } while (number);
    return length;
}

char* strdup(const char* string) {
    size_t length  = strlen(string);
    char*  new_str = malloc(sizeof(char)*(length + 1));
    strcpy(new_str, string);
    return new_str;
}

char* merge_strings(char** strings, char separator, int need_to_free) {
    char* new_string;
    char** ptr = strings;
    size_t total_length = 0, i = 0, j = 0;
    while (*ptr != NULL) { total_length += strlen(*ptr); i++; ptr++; }
    new_string = malloc(sizeof(char)*(total_length + i + 1));
    ptr = strings; i = 0;
    while (*ptr != NULL) {
        while ((*ptr)[i] != '\0') {
            new_string[j++] = (*ptr)[i++]; 
        }
        if (separator != '\0') {
            new_string[j++] = separator; 
        }
        ptr++; i = 0;
    }
    
    new_string[j] = '\0';

    if (need_to_free) {
        ptr = strings;
        while (*ptr != NULL) { free(*ptr); ptr++; }
        free(strings);
    }

    return new_string;
}
