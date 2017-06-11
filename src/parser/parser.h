#ifndef _ZHSH_PARSER_PARSER_H_
#define _ZHSH_PARSER_PARSER_H_

#include <stdlib.h>

#include "token.h"
#include "recognizer.h"
#include "../error.h"
#include "../util.h"
#include "../arraylist.h"
#include "../stringbuilder.h"

typedef enum { FD, PATH, PATH_ENABLE_REWRITE } redirection_type_t;

typedef enum { NEXT_SUCCESS, NEXT_FAIL, NEXT_NOTHING } next_action_t;
typedef enum { FOREGROUND, BACKGROUND                } cmd_execution_mode_t;

typedef struct {
    int from_fd;
    union {
        int   fd;
        char* path;
    } to;
    redirection_type_t type;
} io_redirection_t;

/* ultimate execution unit: built-in or executable program */
typedef struct _cmd_t {
    size_t  argc;
    char**  argv;

    struct _cmd_t* next;
    struct _cmd_t* prev;
} cmd_t;

/* list of binded by pipe commands */
typedef struct _cmd_pipeline {
    size_t      count;
    cmd_t* head;
    cmd_t* tail;

    next_action_t next_action;
    io_redirection_t**  io_redirections;
    struct _cmd_pipeline* next_pipeline;
} cmd_pipeline_t ;

/* chain of binded by logical gates pipelines */
typedef struct {
    cmd_pipeline_t* pipelines;
    cmd_pipeline_t* last_pipe;
    cmd_execution_mode_t execution_mode;
} cmd_chain_t;

arraylist_t* parse(arraylist_t* tokens); /* return list of command chain */
char* chain_to_string(cmd_chain_t* chain);

#endif
