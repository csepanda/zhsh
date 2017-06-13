#include "stringbuilder.h"

#define RESIZE_INCREMENT 255

stringbuilder_t* new_stringbuilder(size_t capacity) {
    stringbuilder_t* builder = malloc(sizeof(stringbuilder_t));
    builder->char_sequence  = calloc(capacity, sizeof(char)); 
    builder->length         = 0;
    builder->capacity       = capacity;

    return builder;
}

void delete_stringbuilder(void* raw_stringbuilder) {
    stringbuilder_t* stringbuilder = raw_stringbuilder;
    free(stringbuilder->char_sequence);
    free(stringbuilder);
}

void append_str(stringbuilder_t* builder, char* str, size_t len) {
    if (builder->length + len >= builder->capacity) {
        size_t cap   = builder->capacity + len + RESIZE_INCREMENT;
        char*  chars = realloc(builder->char_sequence, cap*sizeof(char));
        builder->char_sequence = chars;
        builder->capacity      = cap;
    }
    strncat(builder->char_sequence, str, len);
    builder->length += len;
}

char* stringbuilder_to_string(stringbuilder_t* builder) {
    return strdup(builder->char_sequence);
}
