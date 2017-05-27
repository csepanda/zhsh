#ifndef _ZHSH_ARRAY_LIST_H_
#define _ZHSH_ARRAY_LIST_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

typedef struct {
    size_t size;
    size_t capacity;
    char** data;
} arraylist_str_t;

typedef struct {
    size_t size;
    size_t capacity;
    void   (*destructor)(void*);
    void** data;
} arraylist_t;

arraylist_str_t* new_arraylist_str(size_t capacity);
void remove_arraylist_str         (arraylist_str_t* arraylist);
void persist_to_arraylist_str     (arraylist_str_t* arraylist, char* string);
void merge_to_arraylist_str       (arraylist_str_t* arraylist, char* string); 
char** to_array_str(arraylist_str_t* arraylist);


arraylist_t* new_arraylist   (size_t capacity, void (*destructor)(void*));
void remove_arraylist        (arraylist_t* arraylist);
void persist_to_arraylist    (arraylist_t* arraylist, void* data);
void merge_arraylists        (arraylist_t* dest, arraylist_t* src, size_t pos);

#endif
