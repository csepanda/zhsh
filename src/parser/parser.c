#include "parser.h"

#define NO  0
#define YES 1

void push_cmd(cmd_pipeline_t* pipeline, cmd_t* cmd) {
    if (pipeline->head == NULL) {
        pipeline->head = cmd;
        pipeline->tail = cmd;
    } else {
        cmd->prev = pipeline->tail;
        pipeline->tail->next = cmd;
        pipeline->tail       = cmd;
    }

    pipeline->count++;
}

void push_pipeline(cmd_chain_t* command, cmd_pipeline_t* pipeline) {
    if (command->pipelines == NULL) {
        command->pipelines = pipeline;
        command->last_pipe = pipeline; 
    } else {
        command->last_pipe->next_pipeline = pipeline;
        command->last_pipe = pipeline;
    }
}

void delete_redirection(void* raw_redirection) {
    io_redirection_t* redirection = raw_redirection;
    if (redirection->type == PATH) {
        free(redirection->to.path);
    }

    free(redirection);
}

void delete_cmd_node(void* raw_cmd_node) {
    cmd_t*        cmd_node = raw_cmd_node;
    size_t i; for (i = 0; i < cmd_node->argc; i++) {
        free(cmd_node->argv[i]);
    }


    free(cmd_node->argv);
    free(cmd_node);
}

void delete_cmd_pipeline(void* raw_cmd_pipeline) {
    cmd_pipeline_t*    pipeline = raw_cmd_pipeline;
    cmd_t*             parent   = pipeline->head;
    cmd_t*             child    = parent != NULL ? parent->next : NULL;
    io_redirection_t** redir    = pipeline->io_redirections;
    while (parent != NULL && child != NULL) {
        delete_cmd_node(parent);
        parent = child;
        child = child->next;
    }

    if (redir != NULL) {
        while (*redir != NULL) {
            delete_redirection(*redir++);
        }
        free(pipeline->io_redirections);
    }

    if (parent != NULL) {
        delete_cmd_node(parent);
    }

    free(pipeline);
}

void delete_command(void* raw_command) {
    cmd_chain_t* command = raw_command;
    cmd_pipeline_t* parent  = command->pipelines;
    cmd_pipeline_t* child   = parent != NULL ? parent->next_pipeline : NULL;

    while (parent != NULL && child != NULL) {
        delete_cmd_pipeline(parent);
        parent = child;
        child  = child->next_pipeline;
    }

    if (parent != NULL) {
        delete_cmd_pipeline(parent);
    }

    free(command);
}

cmd_t* create_cmd(arraylist_str_t* args_list) {
    cmd_t* cmd = malloc(sizeof(cmd_t));
    cmd->argc  = args_list->size;
    cmd->argv  = to_array_str(args_list);
    cmd->next  = NULL;
    cmd->prev  = NULL;

    clear_arraylist_str(args_list);

    return cmd;
}

io_redirection_t* create_io_redirection(arraylist_t* tokens, size_t index) {
    char* str; int from_fd = -1;
    io_redirection_t*  io_redir; 
    redirection_type_t redirection_type = PATH;
    token_t* bind  = tokens->data[index];
    token_t* after = tokens->data[index + 1];

    if (after->type == TKN_TYPE_EOF || after->type != TKN_TYPE_BARE_WORD) {
        alarm_msg("zhsh: syntax error");
        return NULL;
    }

    str = after->str;
    if (str[0] == '&') {
        str++; if (digitful(str, after->len - 1)) {
            redirection_type = FD;
        }
    }

    if (redirection_type == PATH && 
        bind->type == TKN_TYPE_OUT_REDIR_REWRITE) {
        redirection_type = PATH_ENABLE_REWRITE;
    }

    if (index != 0) {
        token_t* before = tokens->data[index - 1];
        if (before->type == TKN_TYPE_BARE_WORD) {
            if (digitful(before->str, before->len)) {
                from_fd = atoi(before->str);
            }
        } else {
            tkn_type_t type = before->type;
            if (type != TKN_TYPE_BIND_SEMICOLON && 
                type != TKN_TYPE_BIND_BG) {
                alarm_msg("zhsh: syntax error\n");
                return NULL;
            }
        }
    }
    
    if (from_fd == -1) {
        if (bind->type == TKN_TYPE_IN_REDIR) {
            from_fd = 0;
        } else {
            from_fd = 1;
        }
    }

    io_redir = malloc(sizeof(io_redirection_t));    
    io_redir->from_fd = from_fd;
    if (redirection_type == FD) {
        io_redir->to.fd   = atoi(str);
    } else {
        io_redir->to.path = strdup(str);
    }
    io_redir->type = redirection_type;

    return io_redir;
}


arraylist_t* parse(arraylist_t* tokens) {
    size_t i; token_t* token;
    arraylist_t*     commands        = new_arraylist(2, delete_command);
    arraylist_t*     io_redirections = new_arraylist(2, delete_redirection);
    arraylist_str_t* argv_list       = new_arraylist_str(tokens->size);
    for (i = 0; i < tokens->size; i++) { /* cmd_chain_t level */
        cmd_chain_t* command = calloc(1, sizeof(cmd_chain_t));
        for (; i < tokens->size; i++) { /* cmd_pipeline level */
            cmd_pipeline_t* pipeline = calloc(1, sizeof(cmd_pipeline_t));
            for (; i < tokens->size; i++) { /* cmd_node level */
                token = tokens->data[i];
                if (token->type == TKN_TYPE_BARE_WORD) {
                    merge_to_arraylist_str(argv_list, token->str);
                } else if (is_io_redirection_token(token)) {
                    io_redirection_t* redir = create_io_redirection(tokens, i);
                    if (redir != NULL) {
                        i++; persist_to_arraylist(io_redirections, redir);
                    } else {
                        free(command);
                        free(pipeline);
                        remove_arraylist(commands);
                        remove_arraylist(io_redirections);
                        remove_arraylist_str(argv_list);
                        return NULL;
                    }
                } else if (argv_list->size == 0) {
                    if (token->type != TKN_TYPE_EOF) {
                        break;
                    } else { 
                        free(command);
                        free(pipeline);
                        remove_arraylist(commands);
                        remove_arraylist(io_redirections);
                        remove_arraylist_str(argv_list);
                        if (tokens->size != 1) {
                            alarm_msg("zhsh: syntax error\n");
                        }
                        return NULL;
                    }
                } else {
                    push_cmd(pipeline, create_cmd(argv_list));
                    if (token->type != TKN_TYPE_BIND_PIPE) {
                        break;
                    }
                }
            }

            if (token->type != TKN_TYPE_BIND_PIPE) {
                if (token->type == TKN_TYPE_BIND_AND) {
                    pipeline->next_action = NEXT_SUCCESS;
                } else if (token->type == TKN_TYPE_BIND_OR) {
                    pipeline->next_action = NEXT_FAIL;
                } else {
                    pipeline->next_action = NEXT_NOTHING;
                }
                if (io_redirections->size > 0) {
                    persist_to_arraylist(io_redirections, NULL);
                    pipeline->io_redirections = (io_redirection_t**) 
                                                hard_copy_arraylist(io_redirections);
                } else {
                    pipeline->io_redirections = NULL;
                }

                io_redirections->size = 0;
                push_pipeline(command, pipeline);
                if (token->type == TKN_TYPE_BIND_BG ||
                    token->type == TKN_TYPE_BIND_SEMICOLON ||
                    token->type == TKN_TYPE_EOF) {
                    break;
                }
            }
        }

        if (token->type == TKN_TYPE_BIND_BG) {
            command->execution_mode = BACKGROUND;
        } else if (token->type == TKN_TYPE_BIND_SEMICOLON ||
                   token->type == TKN_TYPE_EOF) {
            command->execution_mode = FOREGROUND;
        }
        persist_to_arraylist(commands, command);


        if (token->type == TKN_TYPE_EOF) {
            break;
        }
    }

    remove_arraylist_str(argv_list);
    remove_arraylist(io_redirections);

    return commands;
}

char* chain_to_string(cmd_chain_t* command) {
    char* result;
    cmd_pipeline_t*  pipeline = command->pipelines;
    stringbuilder_t* builder  = new_stringbuilder(1024);
    while (pipeline != NULL) {
        cmd_t* node = pipeline->head;
        while (node != NULL) {
            char* cmd = merge_strings(node->argv, ' ', 0);
            append_str(builder, cmd, strlen(cmd));
            node = node->next;
            if (node != NULL) {
                append_str(builder, " | ", 3);
            }
            free(cmd);
        }

        if (pipeline->next_action == NEXT_SUCCESS) {
            append_str(builder, " && ", 4);
        } else if (pipeline->next_action == NEXT_FAIL) {
            append_str(builder, " || ", 4);
        }
        pipeline = pipeline->next_pipeline;
    }

    if (command->execution_mode == BACKGROUND) {
        append_str(builder, " &", 2);
    }

    result = stringbuilder_to_string(builder);
    delete_stringbuilder(builder);

    return result;
}
