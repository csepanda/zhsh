#ifndef _ZHYSH_ERROR_H_
#define _ZHYSH_ERROR_H_

#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define PANIC_STDIN_NOTTY "zhysh: stdin isn't linked with any terminal\n"
#define PANIC_TTY_NAVIGATION_INI_POSITION_READ_ERROR "zhysh: tty navigation unit isn't initialized.\n\tCause: cursor position in terminal cannot be readed.\n"

#define PANIC_ZHYSH_INITIALIZATION_ERROR_CODE 2

/* type of exit_code */
typedef uint8_t exit_code_t;


/* function:     panic :: print message to stderr and exit with exit_code
 *     args:   message :: message to print
 *           exit_code :: exit code
 * */
void panic(const char* const message, exit_code_t exit_code);

#endif

