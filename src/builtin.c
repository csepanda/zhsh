#include "builtin.h"

#include "builtins/pwd.c"
#include "builtins/cd.c"
#include "builtins/echo.c"
#include "builtins/kill.c"
#include "builtins/true.c"
#include "builtins/false.c"
#include "builtins/shift.c"
#include "builtins/exit.c"
#include "builtins/jobs.c"
#include "builtins/fg.c"
#include "builtins/bg.c"

builtin_t builtins[] = { 
    { "cd",       cd_builtin }, { "pwd",     pwd_builtin },
    { "echo",   echo_builtin }, { "kill",   kill_builtin },
    { "shift", shift_builtin }, { "exit",   exit_builtin },
    { "true",   true_builtin }, { "false", false_builtin },
    { "jobs",   jobs_builtin }, { "fg",       fg_builtin },
    { "bg",       bg_builtin },
    { NULL, NULL }
};


int exec_builtin_cmd(size_t argc, char** argv) {
    builtin_t* ptr = builtins;
    while ((*ptr).cmd_name != NULL) {
        char* cmd_name = (*ptr).cmd_name;
        if (strcmp(cmd_name, argv[0]) == 0) {
            return (*ptr).cmd_func(argc, argv);
        }
        ptr++;
    }

    return -2;
}
