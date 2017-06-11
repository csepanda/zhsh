#include "interpreter.h"

#include "lexer.h"
#include "parser.h"
#include "preprocessor.h"

#define NO    0
#define YES   1

#define STRHNDL_INITIAL_CAP 4096
#define STRHNDL_AUGMENT      255

void execute_cmd(cmd_t* cmd) {
    char* command_name  = cmd->argv[0];
    builtin_t* built_in = get_builtin(command_name);
    if (built_in == NULL) {
        pid_t child = fork();
        if (child == -1) {
            alarm_msg(ALARM_CANNOT_EXEC);
        } else if (child > 0) {
        } else {
            if (is_path(command_name, strlen(command_name))) {
                execv(command_name, cmd->argv);
                switch (errno) {
                    case  ENOENT:
                    case ENOTDIR:
                        send_errmsg(command_name, ALARM_FILE_NOT_FOUND);
                        _exit(127);
                        break;
                    default:
                        send_errmsg("", ALARM_RUNTIME_ERR);
                }
            } else {
                execvp(command_name, cmd->argv);
                switch (errno) {
                    case  ENOENT:
                    case ENOTDIR:
                        send_errmsg(command_name, ALARM_CMD_NOT_FOUND);
                        _exit(127);
                        break;
                    default:
                        send_errmsg("", ALARM_RUNTIME_ERR);
                }
            }
            _exit(126);
        }
    } else {
        built_in->cmd_func(cmd->argc, cmd->argv);
    }
}

void execute_pipeline(cmd_pipeline_t* pipeline, pid_t pid, 
                         cmd_execution_mode_t mode) {
    int input, output, infd, outfd;
    int backup[2];
    int pipe_fd[2];
    size_t i;
    cmd_t* cmd = pipeline->head;
    backup[0] = dup(0);
    backup[1] = dup(1);
    if (pipeline->io_redirections != NULL) {
        io_redirection_t** redir = pipeline->io_redirections;
        while (*redir != NULL) {
            io_redirection_t*  io   = *redir;
            redirection_type_t type = io->type;
            if (type == FD ) {
                dup2(io->to.fd, io->from_fd);
            } else {
                const int fd_flags = fcntl(io->from_fd, F_GETFL);
                int to_fd;
                int flags = fd_flags & O_RDONLY;
                flags |= fd_flags & O_WRONLY;
                flags |= fd_flags & O_RDWR;
                flags |= flags    & O_RDONLY ? 0 : O_CREAT;
                flags |= type == PATH_ENABLE_REWRITE ? O_TRUNC : O_APPEND;
                if ((to_fd = open(io->to.path, flags)) == -1) {
                    alarm_msg(strerror(errno));
                    _exit(2);
                }

                dup2(to_fd, io->from_fd);
            }
            redir++;
        }
    }

    input  = dup(STDIN);
    output = dup(STDOUT);

    infd = input; outfd = output;

    for (i = 0; i < pipeline->count; i++) {
        dup2(infd, STDIN);
        close(infd);
        if (i != pipeline->count - 1) {
            pipe(pipe_fd);
            infd  = pipe_fd[0];
            outfd = pipe_fd[1];
        } else {
            outfd = output;
        }

        dup2(outfd, STDOUT);
        close(outfd);
        execute_cmd(cmd);
        cmd = cmd->next;
    }

    dup2(backup[0],  STDIN);
    dup2(backup[1], STDOUT);

}

int execute_foreground(cmd_chain_t* command) {
    pid_t child;
    cmd_pipeline_t* pipeline = command->pipelines;
    int   status;
    
    /* if pipeline contains only one command and it's builtin, then
     * execute it in current shell without fork                     */
    if (pipeline->count == 1) {
        cmd_t* cmd     = pipeline->head;
        builtin_t* builtin = get_builtin(cmd->argv[0]);
        if (builtin != NULL) {
            status = builtin->cmd_func(cmd->argc, cmd->argv);
            /* shift while next pipeline execution condition 
             * isn't passing to return status of previous pipeline  */
            while (pipeline != NULL) { 
                next_action_t next = pipeline->next_action;
                pipeline = pipeline->next_pipeline;
                if (!(!!status ^ next)) {
                    break;
                }
            }
            if (pipeline == NULL) {
                return status;
            }
        }
    }

    /* start job */
    if ((child = fork()) == -1) {
        alarm_msg(ALARM_CANNOT_EXEC);
        return -1;
    } else if (child > 0) {
        char* cmd_str = chain_to_string(command);
        int num = add_job(child, cmd_str);
        set_foreground_by_num(num);
    } else {
        pid_t pid = getpid();
        while (pipeline != NULL) {
            size_t fd; int status = 0;
            execute_pipeline(pipeline, pid, FOREGROUND);
            while ((child = waitpid(0, &status, WUNTRACED)) > 0);

            if (WIFSIGNALED(status)) {
                int signum = WTERMSIG(status);
                _exit(128 + signum);
            } else if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                while (pipeline != NULL) { 
                    next_action_t next = pipeline->next_action;
                    pipeline = pipeline->next_pipeline;
                    if (!(!!exit_code ^ next)) {
                        break;
                    }
                }

                if (pipeline == NULL) {
                    for (fd = 3; fd < 256; fd++) {
                        close(fd);
                    }
                    _exit(exit_code);
                }
            } else {
                alarm_msg(ALARM_RUNTIME_ERR);
            }
        }
    }

    return -2; /* exited if job was stopped*/
}

void execute_background(cmd_chain_t* command) {
    pid_t child = fork();
    if (child == -1) {
        alarm_msg(ALARM_CANNOT_EXEC);
        return;
    }
    
    if (child > 0) {
        char* cmd_str = chain_to_string(command);
        int num = add_job(child, cmd_str);
        set_background_by_num(num);
    } else {

    }
}

void main_executor(arraylist_t* commands) {
    size_t i; int fd_backup[255];
    memset(fd_backup, -1, 255);
    for (i = 0; i < commands->size; i++) {
        cmd_chain_t* command = commands->data[i];
        if (command->execution_mode == BACKGROUND) {
            execute_background(command);
        } else {
            execute_foreground(command);
        }
    }
}

void handle_line(char* line, const size_t size) {
    arraylist_t* commands;
    arraylist_t* tokens = tokenize(line, size);
    size_t i; for (i = 0; i < tokens->size; i++) {
        token_t* t = tokens->data[i];
        if (t->type != TKN_TYPE_BARE_WORD) {
            continue;
        }
        if (t->preproccessing_level & TKN_PREPROCESSING_VAR) {
            expand_variables(t);
        }
        if (t->preproccessing_level & TKN_PREPROCESSING_GLOB) {
            i += open_glob(tokens, i);
        }
    }

    commands = parse(tokens);
    if (commands != NULL) {
        main_executor(commands);
        remove_arraylist(commands);
    }

    free(line);
    remove_arraylist(tokens);
}

