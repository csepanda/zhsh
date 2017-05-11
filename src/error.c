#include "error.h"

void panic(const char* const message, exit_code_t exit_code) {
    write(2, message, strlen(message));
    _exit(exit_code);
}
