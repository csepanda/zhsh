#include "env.h"

static char* PS1 = "$ ";
static char* PS2 = "> ";
static char* PS3 = "?# ";
static char* PS4 = "+%N:%i>";

static char* HOME;
static char* LOGNAME;
static char* PWD;
static char* PATH;

static hashtable_t* environment;
extern char** environ;

void env_init() {
    char** envs = environ;
    environment = create_hashtable();
    HOME    = getenv("HOME"   );
    PWD     = getenv("PWD"    );
    PATH    = getenv("PATH"   );
    LOGNAME = getenv("LOGNAME");
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
    return get(environment, key);
}

void debug_env(void) {
    int i = 0;
    hnode_t** nodes = get_hnode_set(environment);
    for (i = 0; i < environment->size; i++) {
        printf("%s -> %s\n", nodes[i]->key, nodes[i]->value);
    }
}
