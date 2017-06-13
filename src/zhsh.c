#include <unistd.h>
#include <stdlib.h>
#include "tty_navigation.h"
#include "parser/interpreter.h"
#include "parser/preprocessor.h"
#include "parser/recognizer.h"
#include "ux/history.h"
#include "ux/complete.h"
#include "jobs.h"
#include "env.h"

typedef enum { INTERACTIVE, COMMAND, SCRIPT } zhsh_mode_t;

static int  buf_index = 0; 
static char buffer[4096];

static tty_navigation_position_t promt_position;

static void zhsh_initialize(zhsh_mode_t mode);
static void zhsh_interactive_mode_start(void);
static  int handle_char(char c);
static void handle_tty_navigation(void);
static void promt(void);

int main(void) {
    zhsh_initialize(INTERACTIVE);
    zhsh_interactive_mode_start();
    return 0;
}

void zhsh_interactive_mode_start(void) {
    char c; for (;;) {
        wait_jobs();
        promt();
        while (getch(&c) && handle_char(c));
    }
}

void zhsh_initialize(zhsh_mode_t mode) {
    env_init();
    job_init();
    if (mode == INTERACTIVE) {
        signal(SIGTTOU, SIG_IGN);
        history_ini(buffer, &buf_index, &promt_position);
        completion_init(buffer, &buf_index);
        set_tty_ncanon();
    }
    set_env("PS1", "$LOGNAME@$HOSTNAME:$PWD \\$ ");
    set_env("IFS", " \t\n");
}

int handle_char(char c) {
    int code;
    switch (c) {
        case ESCAPE_CHAR:
            handle_tty_navigation();
            break;
        case DELETE_CHAR:
            if (buf_index > 0) {
                tty_navigation_move(TTY_MOVE_LEFT);
                write(1, " ", 1);
                buf_index--;
                tty_navigation_move(TTY_MOVE_LEFT);
            }
            break;
        case '\t':
            code = complete();
            if (code == 2) {
                promt(); 
                write(1, buffer, buf_index);
            }
            break;
        default:
            buffer[buf_index++] = c;
            putch(c);
            if (c == '\n') {
                buffer[buf_index - 1] = '\0';
                if (!is_empty(buffer, buf_index - 1)) {
                    history_add_entry(strdup(buffer));
                    handle_line(strdup(buffer), buf_index);
                }
                buf_index = 0;
                
                history_reset();
                return 0;
            }
    }

    return 1;
}


void handle_tty_navigation(void) {
    char c;
    read(0, &c, 1);
    if (c == '[') {
        read(0, &c, 1);
        switch (c) {
        case 'A':
            history_up();
            break;
        case 'B':
            history_down();
            break;
        case 'C':
            tty_navigation_move(TTY_MOVE_RIGHT);
            break;
        case 'D':
            tty_navigation_move(TTY_MOVE_LEFT);
            break;
        }
    }
}

void promt(void) {
    char* PS1 = get_env("PS1");
    char* str = expand_variables_str(PS1);
    write(STDOUT, str, strlen(str));
    tty_navigation_save_position(&promt_position);
    free(str);
}
