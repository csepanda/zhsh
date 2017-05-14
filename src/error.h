#ifndef _ZHYSH_ERROR_H_
#define _ZHYSH_ERROR_H_

#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "io.h"

#define PANIC_STDIN_NOTTY "zhysh: stdin isn't linked with any terminal\n"
#define PANIC_TTY_NAVIGATION_INI_POSITION_READ_ERROR "zhysh: tty navigation unit isn't initialized.\n\tCause: cursor position in terminal cannot be readed.\n"
#define PANIC_ENV_INVALID_VARIABLE_NAME_ERROR  "zhysh: invalid name for environment variable\n"
#define PANIC_ENV_INSUFFUCIENT_MEMORY_ERROR "zhysh: insufficient memory to add a new variable to the environment"
#define PANIC_ENV_UNDEFINED_ERROR "zhysh: environment variables unexpected error"

#define PANIC_ZHYSH_INITIALIZATION_ERROR_CODE      2
#define PANIC_ZHYSH_RUNTIME_ERROR_CODE             3
#define PANIC_ZHYSH_INVALID_USER_INPUT_ERROR_CODE 13

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

