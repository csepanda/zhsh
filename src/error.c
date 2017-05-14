#include "error.h"

void panic(const char* const message, exit_code_t exit_code) {
    write(STDERR, message, strlen(message));
    _exit(exit_code);
}

void alarm_msg(const char* const message) {
    write(STDERR, message, strlen(message));
}
