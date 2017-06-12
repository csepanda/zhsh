#include "error.h"

#define err_write(message) write(STDERR, message, strlen(message))

void panic(const char* const message, exit_code_t exit_code) {
    err_write(message);
    _exit(exit_code);
}

void alarm_errno() {
    err_write("zhsh: ");
    err_write(strerror(errno));
    err_write("\n");
}

void alarm_msg(const char* const message) {
    err_write(message);
}

void send_errmsg(const char* const caller, const char* const cause) {
    err_write("zhsh: ");
    err_write(caller);
    err_write(cause);
}
