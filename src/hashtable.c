#include "hashtable.h"

static const size_t INITIAL_CAPACITY = 1 <<  4;
static const size_t MAXIMUM_CAPACITY = 1 << 30;
static const float  LOAD_FACTOR      = 0.75f;

static uint32_t hash_function(char* str) {
    int c; uint32_t hash = 0;
    while ((c = *str++)) { hash = c + (hash << 6) + (hash << 16) - hash; }
    return hash^(hash >> 16);
}

static hnode_t* new_node(uint32_t hash, char* key, char* value) {
    hnode_t* node = malloc(sizeof(hnode_t));
    node->hash  = hash;
    node->key   = key;
    node->value = value;
    node->next  = NULL;
    return node;
}

static void transfer(hashtable_t* hashtable, hnode_t* node) {
    hnode_t** tab; int32_t i, n;
    uint32_t hash = node->hash;
    tab = hashtable->table;
    n   = hashtable->capacity;

    if ((tab[i = (n - 1) & hash]) == NULL) {
        tab[i] = node;
    } else {
        hnode_t* snode = tab[i];
        while (snode->next != NULL) {
            snode = snode->next;
        }
        
        snode->next = node;
    }
}

static int32_t resize(hashtable_t* hashtable) {
    hnode_t** oldtab = hashtable->table;
    hnode_t** newtab;
    size_t oldcap   = hashtable->capacity;
    size_t oldthr   = hashtable->threshold;
    size_t newcap, newthr = 0;
    if (oldcap > 0) {
        if (oldcap >= MAXIMUM_CAPACITY) {
            hashtable->threshold = -1;
            return oldcap;
        } else if ((newcap = oldcap << 1) < MAXIMUM_CAPACITY &&
                    oldcap >= INITIAL_CAPACITY) {
            newthr = oldthr << 1;
        } 
    } else if (oldthr > 0) {
        newcap = oldthr;
    } else {
        newcap = INITIAL_CAPACITY;
        newthr = INITIAL_CAPACITY*LOAD_FACTOR;
    }

    if (newthr == 0) {
        float ft = (float)newcap * LOAD_FACTOR;
        newthr = (newcap < MAXIMUM_CAPACITY && ft < (float)MAXIMUM_CAPACITY ?
                  (size_t) ft : -1);
    }

    newtab = calloc(newcap, sizeof(hnode_t*));
    hashtable->threshold = newthr;
    hashtable->capacity  = newcap;
    hashtable->table     = newtab;

    if (oldtab != NULL) {
        size_t i;
        for (i = 0; i < oldcap; i++) {
            hnode_t* e;
            if (oldtab[i] == NULL) {
                continue;
            }
            if ((e = oldtab[i])->next == NULL) {
                newtab[e->hash & (newcap - 1)] = e; 
            } else {
                do {
                    hnode_t* next = e->next;
                    e->next = NULL;
                    transfer(hashtable, e);
                    e = next;
                } while (e != NULL);
            }
        }
        free(oldtab);
    }

    return newcap;
}

hashtable_t* create_hashtable() {
    hashtable_t* hashtable = malloc(sizeof(hashtable_t));
    hashtable->table     = NULL;
    hashtable->capacity  = 0;
    hashtable->threshold = 0;
    hashtable->size      = 0;
    return hashtable;
}

void delete_hashtable(hashtable_t* hashtable) {
    hnode_t** tab = hashtable->table;
    if (tab != NULL) {
        int i;
        const int N = hashtable->capacity;
        for (i = 0; i < N; i++) {
            hnode_t* e;
            if ((e = tab[i]) == NULL) {
                continue;
            }
            if (e->next == NULL) {
                free(e);
            } else {
                do {
                    hnode_t* next = e->next;
                    free(e);
                    e = next;
                } while (e != NULL);
            }
            tab[i] = NULL;
        }
        free(tab);
    }
    free(hashtable);
}

void put(hashtable_t* hashtable, char* key, char* value) {
    hnode_t** tab; int32_t i, n;
    uint32_t hash = hash_function(key);
    if ((tab = hashtable->table) == NULL || (n = hashtable->capacity) == 0) {
        n    = resize(hashtable);
        tab  = hashtable->table;
    }

    if ((tab[i = (n - 1) & hash]) == NULL) {
        tab[i] = new_node(hash, key, value);
    } else {
        hnode_t* node = tab[i];
        do {
            if (node->hash == hash && strcmp(node->key, key) == 0) {
                node->value = value;
                return;
            }
        } while (node->next != NULL && (node = node->next));
        node->next = new_node(hash, key, value);
    }

    if (++(hashtable->size) > hashtable->threshold) {
        resize(hashtable);
    }
}

char* get(hashtable_t* hashtable, char* key) {
    hnode_t** tab = hashtable->table;
    hnode_t* node;
    uint32_t hash = hash_function(key);
    uint32_t n    = hashtable->capacity;
    if ((node = tab[((n - 1) & hash)]) == NULL) {
        return NULL;
    }

    if (node->next == NULL) {
        if (node->hash == hash && strcmp(node->key, key) == 0) {
            return node->value;
        } else {
            return NULL;
        }
    } else {
        while (node != NULL) {
            if (node->hash == hash && strcmp(node->key, key) == 0) {
                return node->value;
            }
            node = node->next;
        }

        return NULL;
    }
}

hnode_t** get_hnode_set(hashtable_t* hashtable) {
    size_t i, cap = hashtable->capacity; 
    hnode_t** set = malloc(sizeof(hnode_t*)*hashtable->size);
    hnode_t** ptr = set;
    hnode_t** tab = hashtable->table;
    hnode_t* node;
    for (i = 0; i < cap; i++) {
        if ((node = tab[i]) == NULL) continue;
        if (node->next == NULL) {
            *ptr++ = node;
        } else {
            do {
                *ptr++ = node;
                node   = node->next;
            } while(node != NULL);
        }
    }

    return set;
}

int delete_hnode(hashtable_t* hashtable, char* key) {
    hnode_t** tab = hashtable->table;
    hnode_t* node;
    hnode_t* parent;
    uint32_t hash  = hash_function(key);
    uint32_t n     = hashtable->capacity;
    uint32_t index; 
    if ((node = tab[(index = ((n - 1) & hash))]) == NULL) {
        return -1;
    }

    if (node->next == NULL) {
        tab[index] = NULL;
        free(node);
    } else if (node->hash == hash && strcmp(node->key, key) == 0) {
        tab[index] = node->next;
        free(node);
    } else {
        parent = node;
        node   = node->next;
        while (strcmp(node->key, key) != 0) {
            parent = node;
            node   = node->next;
            if (node == NULL) {
                return -1;
            }
        }
        parent->next = node->next;
        free(node);
    }

    return 1;
}
