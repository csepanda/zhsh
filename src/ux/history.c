#include "history.h"

#define OFLAGS O_RDWR     | O_CREAT
#define MFLAGS PROT_WRITE | PROT_READ

static char*                      buffer;
static int*                       buffer_index_ptr;
static tty_navigation_position_t* promt_position_ptr;

static int              last_fetched;
static arraylist_str_t* history;
static size_t           history_ptr;


static int open_history_file() {
    int   fd;
    char* home = get_env("HOME");
    char  history_file[1024];
    history_file[0] = '\0';
    strcat(history_file, home);
    strcat(history_file, "/.zhsh_history");
    if ((fd = open(history_file, OFLAGS, (mode_t)0600)) == -1) {
        alarm_msg("zhsh: history restoring or saving is unavailable\n");
        alarm_errno();
        return -1;
    }

    return fd;
}

#define BUFSIZE 4096
static int read_history_file() {
    int    fd = open_history_file();
    char   buffer[BUFSIZE];
    size_t readed; 

    if (fd < 0) {
        return -1;
    }

    while ((readed = read(fd, buffer, BUFSIZE)) > 0) {
        size_t i, j = 0;
        for (i = 0; i < readed; i++) {
            if (buffer[i] == '\n') {
                buffer[i] = '\0';
                merge_to_arraylist_str(history, buffer + j);
                j = i + 1;
            }
        }

        if (readed == BUFSIZE && buffer[readed - 1] != '\0') {
            lseek(fd, j - readed, SEEK_END);
        }
    } 

    if (readed == -1) {
        alarm_msg("zhsh: error occurred while history file reading\n");
        alarm_errno();
        return -1;
    }

    last_fetched = history->size;
    history_ptr  = last_fetched;

    close(fd);

    return 0; 
}

void history_save() {
    int fd = open_history_file();
    const char LN = '\n';
    size_t i, write_status = 1337;
    if (fd < 0) {
        return;
    } 

    for (i = 0; i < history->size && write_status > 0; i++) {
        char* line = history->data[i];
        write_status = write(fd, line, strlen(line));
        write_status = write(fd, &LN, 1);
    }
    
    if (write_status == -1) {
        alarm_msg("zhsh: error occured while history file writing\n");
        alarm_errno();
        return;
    }

    close(fd);

    return;

}


void history_ini(char* buf_ptr, int* buf_index_ptr, 
                 tty_navigation_position_t* promt_position) {     
    buffer             = buf_ptr;
    buffer_index_ptr   = buf_index_ptr;
    promt_position_ptr = promt_position;
    history            = new_arraylist_str(255);
    read_history_file();
}

void history_reset() {
    history_ptr = history->size;
}

void history_add_entry(char* line) {
    persist_to_arraylist_str(history, line);
}

void history_down() {
    char* line; 
    size_t i, len;
    if (history_ptr >= history->size || history->size == 0) {
        return;
    }

    tty_navigation_move_to(*promt_position_ptr);
    clear_to_end;
    line = history->data[++history_ptr - 1];
    len  = strlen(line);
    write(1, line, len);
    for (i = 0; i < len; i++) {
        buffer[i] = line[i];
    }
    *buffer_index_ptr = i;
}

void history_up() {
    char* line; 
    size_t i, len;
    if (history_ptr < 1) {
        return;
    }

    tty_navigation_move_to(*promt_position_ptr);
    clear_to_end;
    line = history->data[history_ptr-- - 1];
    len  = strlen(line);
    write(STDOUT, line, len);
    for (i = 0; i < len; i++) {
        buffer[i] = line[i];
    }
    *buffer_index_ptr = i;
}
