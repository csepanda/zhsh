#include "tty_navigation.h"

static void tty_navigation_object_ini_position(void);

static tty_navigation_position_t* cp = &tty_navigation.current_position;
static tty_navigation_position_t* ip = &tty_navigation.user_input_position;

void tty_navigation_object_ini(void) {
    tty_navigation_object_ini_position();
}

void tty_navigation_line_down(void) {
    write(1, "[1E", 4);
}

void tty_navigation_line_up(void) {
    write(1, "[1F", 4);
}

void tty_navigation_save_pos_native(void) {
    write(1, "[s", 3);
}

void tty_navigation_restore_pos_native(void) {
    write(1, "[u", 3);
}

void tty_navigation_move(tty_navigation_direction_t direction) {
    switch (direction) {
    case TTY_MOVE_UP:
        write(1, TTY_MOVE_UP_STR,    4);
        if (cp->y > 1) cp->y--;
        break;
    case TTY_MOVE_DOWN:
        write(1, TTY_MOVE_DOWN_STR,  4);
        cp->y++;
        break;
    case TTY_MOVE_RIGHT:
        write(1, TTY_MOVE_RIGHT_STR, 4);
        cp->x++;
        break;
    case TTY_MOVE_LEFT:
        write(1, TTY_MOVE_LEFT_STR,  4);
        if (cp->x > 1) cp->x--;
        break;
    }
}

void tty_navigation_move_to(tty_navigation_position_t position) {
    int i = 2;
    char buffer[32];
    buffer[0] = 27;
    buffer[1] = '[';
    i += int_to_string(position.y, buffer + i);
    buffer[i++] = ';';
    i += int_to_string(position.x, buffer + i);
    buffer[i++] = 'H';
    buffer[i] = '\0';
    write(STDOUT, buffer, i);
    cp->y = position.y;
    cp->x = position.x;
}

void tty_navigation_print_debug_info(void) {
    puts("TTY NAVIGATION OBJECT {");
    puts("\tcurrent position {");
    printf("\t\tx: %i\n\t\ty: %i\n\t}\n", cp->x, cp->y);
    puts("\tuser's input position {");
    printf("\t\tx: %i\n\t\ty: %i\n\t}\n", ip->x, ip->y);
    puts("}");
}

void tty_navigation_save_position(tty_navigation_position_t* pos) {
    int i = 0; char buffer[32];

    /* 
     * CSI 6n :: device status report
     * return to stdin the cursor position in format ESC[n;mR
     *      n :: row
     *      m :: column
     */
    write(1, "[6n", 4); 

    read(0, buffer, 2);
    if (buffer[0] != '' || buffer[1] != '[') {
        panic(PANIC_TTY_NAVIGATION_INI_POSITION_READ_ERROR, 
              PANIC_ZHSH_INITIALIZATION_ERROR_CODE);
    }
    
    while (read(0, (buffer + i++), 1) == 1) {
        if (buffer[i - 1] == ';') {
            buffer[i - 1] = '\0';
            pos->y = atoi(buffer);
            i = 0;
        } else if (buffer[i - 1] == 'R') {
            buffer[i - 1] = '\0';
            pos->x = atoi(buffer);
            break;
        }
    }

}


/* initialize 'tty_navigation_position's that refers the cursor position tty */
static void tty_navigation_object_ini_position(void) {
    int i = 0; char buffer[32];

    /* 
     * CSI 6n :: device status report
     * return to stdin the cursor position in format ESC[n;mR
     *      n :: row
     *      m :: column
     */
    write(1, "[6n", 4); 

    read(0, buffer, 2);
    if (buffer[0] != '' || buffer[1] != '[') {
        panic(PANIC_TTY_NAVIGATION_INI_POSITION_READ_ERROR, 
              PANIC_ZHSH_INITIALIZATION_ERROR_CODE);
    }
    
    while (read(0, (buffer + i++), 1) == 1) {
        if (buffer[i - 1] == ';') {
            buffer[i - 1] = '\0';
            cp->y = atoi(buffer);
            ip->y = cp->y;
            i = 0;
        } else if (buffer[i - 1] == 'R') {
            buffer[i - 1] = '\0';
            cp->x = atoi(buffer);
            ip->x = cp->x;
            break;
        }
    }

}
