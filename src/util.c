#include "util.h"

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
    char*  new_str = malloc(sizeof(char)*length);
    strcpy(new_str, string);
    return new_str;
}
