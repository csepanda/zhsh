#ifndef _ZHSH_STRINGBUILDER_H_
#define _ZHSH_STRINGBUILDER_H_

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

typedef struct {
    char*  char_sequence;
    size_t length;
    size_t capacity;
} stringbuilder_t;

stringbuilder_t* new_stringbuilder(size_t capacity);
void delete_stringbuilder(void* raw_stringbuilder);
void append_str(stringbuilder_t* builder, char* str, size_t len);

char* stringbuilder_to_string(stringbuilder_t* stringbuilder);
#endif

