#include "env.h"

static size_t argc;
static char** argv;

static hashtable_t* environment;

extern char** environ;


void env_init(size_t arc, char** arv) {
    char** envs = environ;
    environment = create_hashtable();
    argc = arc;
    argv = arv;
    while (*envs != NULL) {
        char* entry = strdup(*envs++);
        char* split = entry;
        while(*++split != '=');
        *split++ = '\0';
        put(environment, entry, split);
    }
}

int set_env(char* key, char* value) {
    if (key == NULL              || key[0] == '\0' || 
        strchr(key, '=') != NULL || value == NULL) {
        alarm_msg(PANIC_ENV_INVALID_VARIABLE_NAME_ERROR); 
        errno = EINVAL;
        return -1;
    }

    put(environment, key, value);
    return 1;
}

int unset_env(char* key) {
    if (key == NULL || strchr(key, '=') != NULL) {
        alarm_msg(PANIC_ENV_INVALID_VARIABLE_NAME_ERROR); 
        return -1;
    }

    delete_hnode(environment, key);
    return 1;
}

char* get_env(char* key) {
    if (str_is_num(key)) {
        size_t n = atol(key);
        return n < argc ? argv[n] : NULL;
    }
    return get(environment, key);
}

int shift_pos_args(size_t count) {
    size_t i;

    if (count >= argc) {
        argc = 1;
        return 0;
    }

    for (i = count; i < argc - 1; i++) {
        argv[i - count + 1] = argv[i + 1];
    }

    argc = argc - count;
    return 0;
}

void debug_env(void) {
    int i = 0;
    hnode_t** nodes = get_hnode_set(environment);
    for (i = 0; i < environment->size; i++) {
        char* key = nodes[i]->key;
        char* val = nodes[i]->value;
        write(STDOUT, key, strlen(key));
        write(STDOUT, " -> ", 4);
        write(STDOUT, val, strlen(val));
        write(STDOUT, "\n", 1);
    }
}
