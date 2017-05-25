#ifndef _ZHSH_HASHTABLE_H_
#define _ZHSH_HASHTABLE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct _hnode_t {
    uint32_t hash;
    char*   value;
    char*   key;
    struct _hnode_t* next;
} hnode_t;

typedef struct _hashtable_t {
    hnode_t** table;
    size_t    size;
    size_t    threshold;
    size_t    capacity;
} hashtable_t;

hashtable_t* create_hashtable();
void         delete_hashtable(hashtable_t* hashtable);

hnode_t**    get_hnode_set   (hashtable_t* hashtable);
int          delete_hnode    (hashtable_t* hashtable, char* key);
char*        get             (hashtable_t* hashtable, char* key);
void         put             (hashtable_t* hashtable, char* key, char* value);
#endif
