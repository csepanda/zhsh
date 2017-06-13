#include "complete.h"

#define YES 1
#define NO  0
#define MIN_RESULTS_MORE 128

static char* buffer;
static int*  buffer_index_ptr;

static arraylist_str_t* executables;
static arraylist_str_t* result_set;

static int _strcmp(const void* a, const void* b) {
    const char** str_a = (const char**) a;
    const char** str_b = (const char**) b;
    return strcmp(*str_a, *str_b);
}

static void read_executables() {
    char* PATH = get_env("PATH");
    char* path = strtok(PATH, ":");
    char buffer[4096]; 
    size_t path_length = 0;
    while (path != NULL) {
        DIR* dir = opendir(path);
        struct dirent* ent;
        if (dir == NULL) { continue; }
        strcpy(buffer, path);
        strcat(buffer, "/");
        path_length = strlen(buffer);
        while ((ent = readdir(dir)) != NULL) {
            char* name = ent->d_name;
            struct stat file_stat;
            buffer[path_length] = '\0';
            strcat(buffer, name);
            stat(buffer, &file_stat);
            if (stat(buffer, &file_stat) == -1   ||
                !(S_ISREG(file_stat.st_mode))    ||
                !(file_stat.st_mode & S_IXUSR    ||
                  file_stat.st_mode & S_IXGRP    ||
                  file_stat.st_mode & S_IXOTH)) {
                continue;
            }
            merge_to_arraylist_str(executables, name);
        }

        path = strtok(NULL, ":");
        closedir(dir);
    }
    qsort(executables->data, executables->size, sizeof(char*), _strcmp);
}

static void read_dirents(char* str, size_t len) {
    char* path;
    char  CUSTOM_PATH = YES;
    struct dirent* ent; DIR* dir;
    size_t i, j;
    path = strdup(str); 
    i = last_slash(path, len);
    if (is_path(path, len)) {
        path[i + 1] = '\0';
    } else {
        CUSTOM_PATH = NO;
        free(path);
        path = ".";
    }

    result_set->size = 0;
    if ((dir = opendir(path)) == NULL) {
        if (CUSTOM_PATH) { free(path); }
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        char* name = ent->d_name;
        if (CUSTOM_PATH) {
            char buffer[512];
            strcpy(buffer, path);
            if (CUSTOM_PATH && path[i] != '/') {
                strcat(buffer, "/");
            }
            strcat(buffer, name);
            name = buffer;
        }
        for (j = 0; j < len && name[j] == str[j]; j++);
        if (j == len) {
            merge_to_arraylist_str(result_set, name);
        }
    }

    closedir(dir);
    if (CUSTOM_PATH)
        free(path);
}

static void find(arraylist_str_t* list, char* str) {
    size_t i, j, len; char* node; 
    size_t str_len = strlen(str);
    size_t left = 0, right = list->size;
    result_set->size = 0;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        node = list->data[mid];
        len  = strlen(node);
        for (j = 0; j < len && j < str_len; j++) {
            if (node[j] > str[j]) {
                right = mid;
                break;
            } else if (node[j] < str[j]) {
                left = mid + 1;
                break;
            }
        }
        if (right == mid || left == mid + 1) {
            continue;
        }
        if (j == str_len) {
            persist_to_arraylist_str(result_set, node);
            for (i = mid + 1; i < right; i++) {
                node = list->data[i];
                for (j = 0; j < len && j < str_len && node[j] == str[j]; j++);
                if (j == str_len) {
                    persist_to_arraylist_str(result_set, node);
                } else {
                    break;
                }
            }

            for (i = mid - 1; i > left; i--) {
                node = list->data[i];
                for (j = 0; j < len && j < str_len && node[j] == str[j]; j++);
                for (j = 0; j < len && j < str_len && node[j] == str[j]; j++);
                if (j == str_len) {
                    persist_to_arraylist_str(result_set, node);
                } else {
                    break;
                }
            }
            
            qsort(result_set->data, result_set->size, sizeof(char*), _strcmp);
            return;
        } else {
            left = mid + 1;
            continue;
        }
    }
}

void completion_init(char* buffer_ptr, int* buffer_ind_ptr) {
    executables = new_arraylist_str(512);
    result_set  = new_arraylist_str(255);
    read_executables();
    buffer = buffer_ptr;
    buffer_index_ptr = buffer_ind_ptr;
}

int more_promt(void) {
    char buffer[255];
    size_t count = result_set->size;
    strcpy(buffer, "Display all ");
    int_to_string(count, buffer + strlen(buffer));  
    strcat(buffer, " possibilities? (y or n) ");
    write(1, buffer, strlen(buffer));
    while (read(0, buffer, 1)) {
        if (buffer[0] == 'y') {
            return 1;
        } else if (buffer[0] == 'n') {
            return 0;
        }
    }

    return -1;
}

void insert_minimal_prefix(size_t count, size_t min_len, size_t l) {
    size_t i, j, k;
    char   prefix[512];
    if (min_len <= l) {
        return;
    }

    strcpy(prefix, result_set->data[0]);
    j = min_len;
    puts(prefix);
    for (i = 1; i < count; i++) {
        char*  str = result_set->data[i];
        for (k = j; k >= l; k--) {
            if (prefix[k] != str[k]) {
                j--;
            }
        }

        if (j < l) {
            break;
        }
    }

    k = *buffer_index_ptr;
    if (j >= l) {
        for (i = l - 1; i <= j; i++) {
            buffer[*buffer_index_ptr] = prefix[i];
            *buffer_index_ptr += 1;
        }
    }
    if (*buffer_index_ptr != k)
        *buffer_index_ptr -= 1;
}

int complete(void) {
    size_t i, start = -1, size;
    if (*buffer_index_ptr == 0) {
        return -1;
    }
    buffer[*buffer_index_ptr] = '\0';
    for (i = 0; i < *buffer_index_ptr; i++) {
        if (buffer[i] == ' ') {
            start = i;
        }
    }

    if (start == -1) {
        find(executables, buffer);
    } else {
        read_dirents(buffer + start + 1, strlen(buffer + start + 1));
    }

    size   = result_set->size;
    putch("\n");
    tty_navigation_line_down();
    if (size > MIN_RESULTS_MORE) {
        if (more_promt() == 1) {
            int pipe_fd[2];
            pid_t child;
            pipe(pipe_fd);
            reset_tty();
            if ((child = fork())) {
                close(pipe_fd[0]);
                for (i = 0; i < size; i++) {
                    char* cmd = result_set->data[i++];
                    write(pipe_fd[1], cmd, strlen(cmd));
                    write(pipe_fd[1], "\n", 1);
                }
                close(pipe_fd[1]);
                waitpid(child, pipe_fd, 0);
                setup_tty();
                return 2;
            } else {
                close(pipe_fd[1]);
                dup2(pipe_fd[0], 0);
                close(pipe_fd[0]);
                execlp("more", "more", NULL);
            }
        } else {
            putch("\n");
            return 2;
        }
    } else {
        size_t max_len = 0, min_len = 13371488;
        for (i = 0; i < size; i++) {
            size_t len = strlen(result_set->data[i]);
            max_len = max_len < len ? len : max_len;
            min_len = min_len > len ? len : min_len;
        }
        for (i = 0; i < size; i++) {
            char*  str = result_set->data[i];
            size_t len = strlen(str);
            write(0, str, len);
            if (max_len < 34 && i + 1 != size) {
                size_t j; for (j = 0; j < max_len - len + 2; j++) {
                    putch(" ");
                }
                str = result_set->data[++i];
                len = strlen(str);
                write(0, str, len);
            }
            putch("\n");
        } 

        if (size) {
            insert_minimal_prefix(size, min_len, *buffer_index_ptr-start);
        }

        return 2;
    }
    
    return 0;
}
