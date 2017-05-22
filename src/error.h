#ifndef _ZHSH_ERROR_H_
#define _ZHSH_ERROR_H_

#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "io.h"

#define PANIC_STDIN_NOTTY "zhsh: stdin isn't linked with any terminal\n"
#define PANIC_TTY_NAVIGATION_INI_POSITION_READ_ERROR "zhsh: tty navigation unit isn't initialized.\n\tCause: cursor position in terminal cannot be readed.\n"
#define PANIC_ENV_INVALID_VARIABLE_NAME_ERROR  "zhsh: invalid name for environment variable\n"
#define PANIC_ENV_INSUFFUCIENT_MEMORY_ERROR "zhsh: insufficient memory to add a new variable to the environment\n"
#define PANIC_ENV_UNDEFINED_ERROR "zhsh: environment variables unexpected error\n"

#define ALARM_CD_EACCES "zhsh: cannot change directory. Permission denied\n"
#define ALARM_CD_EFAULT "zhsh: cannot change directory. Illegal address\n"
#define ALARM_CD_EIO    "zhsh: cannot change directory. I/O error occured while reading from or writing to the file system\n"
#define ALARM_CD_ELOOP  "zhsh: cannot change directory. Too many symbolic links were encountered in specified path\n"
#define ALARM_CD_ENODIR "zhsh: cannot change directory. No such directory\n"
#define ALARM_CD_DEF    "zhsh: cannot change directory\n"

#define ALARM_PWD_EACCES "zhsh: cannot print current directory. Permission denied\n"
#define ALARM_PWD_DEF "zhsh: cannot print current directory\n"

#define PANIC_ZHSH_INITIALIZATION_ERROR_CODE      2
#define PANIC_ZHSH_RUNTIME_ERROR_CODE             3
#define PANIC_ZHSH_INVALID_USER_INPUT_ERROR_CODE 13

/* type of exit_code */
typedef uint8_t exit_code_t;


/* function:     panic :: print message to stderr and exit with exit_code
 *     args:   message :: message to print
 *           exit_code :: exit code
 * */
void panic(const char* const message, exit_code_t exit_code);

/* function: alarm_msg :: print message to stderr
 *     args:   message :: message to print
 * */
void alarm_msg(const char* const message);

#endif

