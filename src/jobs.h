#ifndef _ZHSH_JOBS_H
#define _ZHSH_JOBS_H

#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "arraylist.h"
#include "tty_handle.h"
#include "util.h"
#include "sig.h"
#include "io.h"

typedef enum { RUNNING, DONE, STOPPED, TERMINATED } job_stat_t;

/* encapsulate bunch of commands that executed in one parent group */
typedef struct _job {
    pid_t        pid;   /* actually it's also pgid */
    char*        cmd;
    size_t       num;   /* job's number in jobs list */

    job_stat_t stat; 
    union {
        int exit_code;
        int signal_number;
    } ret;

    struct _job* next;
} job_t;

typedef struct {
    size_t size;
    job_t* head;
    job_t* tail;
} job_list_t;

void job_init();
void print_all_jobs();

int add_job(pid_t pid, char* cmd);

int wait_jobs();
int set_foreground_by_num(size_t num);
int set_background_by_num(size_t num);

int set_foreground_last_updated_job();
int set_background_last_updated_job();

#endif
