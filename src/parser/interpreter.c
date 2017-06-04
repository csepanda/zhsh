#include "interpreter.h"

#include "lexer.h"
#include "preprocessor.h"

#define NO    0
#define YES   1

#define ESCAPE_LEVEL_NOESC         0
#define ESCAPE_LEVEL_QUOTES        1
#define ESCAPE_LEVEL_DOUBLE_QUOTES 2

#define STRHNDL_INITIAL_CAP 4096
#define STRHNDL_AUGMENT      255

#define TOKEN_PROCESSING_NO   0
#define TOKEN_PROCESSING_VAR  1
#define TOKEN_PROCESSING_GLOB 2
#define TOKEN_PROCESSING_BOTH 3

typedef enum { NONE, AND, OR, PIPE    } bind_type_t;
typedef enum { FOREGROUND, BACKGROUND } work_mode_t;

typedef struct _cmd_node_t {
    size_t  argc;
    char**  argv;

    bind_type_t linkage;
    struct _cmd_node_t* next;
} cmd_node_t;

typedef struct {
    size_t      count;
    cmd_node_t* head;
    cmd_node_t* tail;
} cmd_queue_t;

void push(cmd_queue_t* queue, cmd_node_t* cmd) {
    if (queue->head == NULL) {
        queue->head = cmd;
        queue->tail = cmd;
    } else {
        queue->tail->next = cmd;
    }
}

static void execute(cmd_queue_t* cmds) {

}

static cmd_queue_t split_to_cmds(arraylist_t* tokens) {
    size_t i; 
    cmd_queue_t queue;
    cmd_node_t* cmd = malloc(sizeof(cmd_node_t));
    queue.count = 0;
    queue.head  = NULL;
    queue.tail  = NULL;
    for (i = 0; i < tokens->size; i++) {
        
    }

    return queue;
}

void handle_line(char* line, const size_t size) {
    char* IFS = get_env("IFS");
    char* token;
    char** to_merge;
    char** args;
    size_t i, j;
    arraylist_t* tokens = tokenize(line, size);
    arraylist_str_t* args_list;
    for (i = 0; i < tokens->size; i++) {
        token_t* t = tokens->data[i];
        if (t->preproccessing_level & TOKEN_PROCESSING_VAR) {
            expand_variables(t);
        }
        if (t->preproccessing_level & TOKEN_PROCESSING_GLOB) {
            open_glob(tokens, i);
        }
    }

    to_merge = malloc(sizeof(char*)*(tokens->size + 1));
    for (i = 0, j = 0; i < tokens->size; i++) {
        if (((token_t*)tokens->data[i])->type == TKN_TYPE_BARE_WORD) {
            to_merge[j++] = ((token_t*)tokens->data[i])->str;
        }
    }
    to_merge[j] = NULL;
    line = merge_strings(to_merge, ' ', NO);

    token     = strtok(line, IFS);
    args_list = new_arraylist_str(2 + tokens->size);
    while (token != NULL) {
        merge_to_arraylist_str(args_list, token);
        token     = strtok(NULL, IFS);
    } 


    if (args_list->size > 0) {
        size_t argc = args_list->size;
        args = to_array_str(args_list);
        if (exec_builtin_cmd(argc, args) != -2) {
            size_t i; for (i = 0; i < argc; i++) {
                free(args[i]);
            }
            free(args);
        } else {
            pid_t child = fork();
            if (child == -1) {
                alarm_msg(ALARM_CANNOT_EXEC);
            } else if (child > 0) {
                int num = add_job(child, argc, args);
                set_foreground_by_num(num);
            } else if (is_path(args[0], strlen(args[0]))) {
                reset_tty();
                execv(args[0], args);
                switch (errno) {
                case  ENOENT:
                case ENOTDIR:
                    send_errmsg(args[0], ALARM_FILE_NOT_FOUND);
                    break;
                default:
                    send_errmsg("", ALARM_RUNTIME_ERR);
                }
                _exit(127);
            } else {
                reset_tty();
                if (strchr(line, '&') != NULL) {
                }
                execvp(args[0], args);
                switch (errno) {
                case  ENOENT:
                case ENOTDIR:
                    send_errmsg(args[0], ALARM_CMD_NOT_FOUND);
                    break;
                default:
                    send_errmsg("", ALARM_RUNTIME_ERR);
                }
                _exit(127);
            }
        }
    }

    free(line);
    free(to_merge);
    remove_arraylist(tokens);
    remove_arraylist_str(args_list);
}

