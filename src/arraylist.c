#include "arraylist.h"

arraylist_str_t* new_arraylist_str(size_t capacity) {
    arraylist_str_t* arraylist = malloc(sizeof(arraylist_str_t));
    if (capacity == (size_t) -1) {
        capacity = 10;
    }
    arraylist->size     = 0;
    arraylist->capacity = capacity;
    arraylist->data     = malloc(sizeof(char*)*capacity);

    return arraylist;
}

void remove_arraylist_str(arraylist_str_t* arraylist) {
    size_t i; for (i = 0; i < arraylist->size; i++) {
        free(arraylist->data[i]);
    }

    free(arraylist);
}

void persist_to_arraylist_str(arraylist_str_t* arraylist, char* string) {
    if (arraylist->size + 1 >= arraylist->capacity) {
        char** reallocated = realloc(arraylist->data, arraylist->capacity*2);
        arraylist->data = reallocated;
    }
    arraylist->data[arraylist->size++] = string;
}

void merge_to_arraylist_str(arraylist_str_t* arraylist, char* string) {
    persist_to_arraylist_str(arraylist, strdup(string));
}

arraylist_t* new_arraylist(size_t capacity) {
    arraylist_t* arraylist = malloc(sizeof(arraylist_t));
    if (capacity == (size_t) -1) {
        capacity = 10;
    }
    arraylist->size     = 0;
    arraylist->capacity = capacity;
    arraylist->data     = malloc(sizeof(void*)*capacity);

    return arraylist;
}

void remove_arraylist(arraylist_t* arraylist) {
    size_t i; for (i = 0; i < arraylist->size; i++) {
        free(arraylist->data[i]);
    }

    free(arraylist);
}

void persist_to_arraylist(arraylist_t* arraylist, void* data) {
    if (arraylist->size + 1 >= arraylist->capacity) {
        void** reallocated = realloc(arraylist->data, arraylist->capacity*2);
        arraylist->data = reallocated;
    }
    arraylist->data[arraylist->size++] = data;
}

void merge_arraylists(arraylist_t* dest, arraylist_t* src, size_t pos) {
    size_t i; if (dest->size + src->size >= dest->capacity) {
        void** reallocated = realloc(dest->data, dest->capacity + src->size);
        dest->data = reallocated;
    }

    if (dest->size > pos) {
        for (i = dest->size - 1; i >= pos; i--) {
            dest->data[i + src->size] = dest->data[i];
        }
    }

    for (i = 0; i < src->size; i++) {
        dest->data[i + pos] = src->data[i];
    }
}