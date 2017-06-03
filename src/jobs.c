#include "jobs.h"

static job_list_t* jobs;
static size_t last_updt_job = -1;
static pid_t  SHELL_PID;

job_t* new_job(pid_t pid, char* cmd) {
    job_t* job = malloc(sizeof(job_t));
    job->pid     = pid;
    job->cmd     = cmd;
    job->num     = jobs->tail == NULL ? 0 : jobs->tail->num + 1;
    job->stat    = RUNNING;
    job->extcode = 1337;
    job->signum  = 1337;
    job->next    = NULL;
    setpgid(pid, pid);
    last_updt_job = job->num;

    return job;
}

void delete_job(void* raw_job) {
    job_t* job = (job_t*) raw_job;
    free(job->cmd);
    free(job);
}

void job_init() {
    jobs = malloc(sizeof(job_list_t));
    jobs->size = 0;
    jobs->head = NULL;
    jobs->tail = NULL;
    SHELL_PID = getpid();
}

int add_job(pid_t pid, char* cmd) {
    job_t* job = new_job(pid, cmd);
    if (jobs->head == NULL) {
        jobs->head = job;
        jobs->tail = job;
    } else {
        jobs->tail->next = job;
        jobs->tail = job;
    }
    jobs->size++;
    return job->num;
}

static job_t* get_job(size_t num) {
    job_t* job = jobs->head;
    while (job != NULL) {
        if (job->num == num) {
            return job;
        }
        job = job->next;
    }

    return NULL;
}

static int remove_job(job_t* job) {
    job_t* parent = jobs->head;
    job_t* child  = parent;
    while (child != NULL) {
        if (child == job) {
            if (child == parent) {
                if (child != jobs->tail) {
                    jobs->head = child->next;
                } else {
                    jobs->head = NULL;
                    jobs->tail = NULL;
                }
            } else if (child == jobs->tail) {
                jobs->tail = parent;
            } else {
                parent->next = child->next;
            }
            delete_job(child);
            jobs->size--;
            return 0;
        }
        parent = child;
        child  = child->next;
    }

    return -1;
}

static char* get_string_status(job_stat_t status) {
    switch (status) {
    case RUNNING:
        return "RUNNING";
    case STOPPED:
        return "STOPPED";
    case TERMINATED:
        return "TERMINATED";
    case DONE:
        return "DONE";
    default:
        return "UNDEF";
    }
}

static void write_status(job_t* job) {
    char  number[32];
    char  exit_code[32];
    char* output;
    char* status;
    size_t num_length = int_to_string(job->num, number);
    size_t ecd_length = int_to_string(job->extcode, exit_code);
    size_t cmd_length = strlen(job->cmd);
    size_t out_length = 32 + num_length + cmd_length + ecd_length;
    output = calloc(out_length, sizeof(char));
    status = get_string_status(job->stat);
    strcat(output, "[");
    strcat(output, number);
    strcat(output, "]\t");
    strcat(output, status);

    if (job->signum != 1337) {
        strcat(output, "(");
        strcat(output, get_sig(job->signum).signame); 
        strcat(output, ")\t\t");
    } else if (job->stat == DONE && job->extcode != 0 && job->extcode != 1337) {
        strcat(output, "(");
        strcat(output, exit_code);
        strcat(output, ")\t\t");
    } else {
        strcat(output, "\t\t");
    }

    strcat(output, job->cmd);
    strcat(output, "\n\0");

    write(1, output, strlen(output));
    free(output);
}

void print_all_jobs() {
    job_t* job = jobs->head;
    while (job != NULL) {
        write_status(job);
        job = job->next;
    }
}

int wait_jobs() {
    job_t* job = jobs->head;
    job_t* next;
    while (job != NULL) {
        int status;
        pid_t ret = waitpid(job->pid, &status, WNOHANG);
        next = job->next;
        if (ret < 0) {
            remove_job(job);
        } else if (ret == job->pid) {
            job->extcode = status;
            job->stat    = DONE;
            write_status(job);
            remove_job(job);
        }
        job = next;
    }

    return 0;
}


int set_foreground_by_num(size_t num) {
    job_t* job = get_job(num);
    int status;
    if (job == NULL) {
        send_errmsg(ALARM_FG, ALARM_NO_SUCH_JOB); 
        return -1;
    }
    
    tcsetpgrp(STDIN, job->pid);

    if (job->stat == STOPPED) {
        reset_tty();
        kill(-job->pid, SIGCONT);
        job->stat = RUNNING;
        write_status(job);
    }

    waitpid(job->pid, &status, WUNTRACED);
    if (WIFEXITED(status)) {
        job->extcode = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        job->stat   = TERMINATED;
        job->signum = WTERMSIG(status);
    }

    if (WIFSTOPPED(status)) {
        job->stat = STOPPED;
        write_status(job);
        last_updt_job = job->num;
    } else {
        job->stat = DONE;
    }

    tcsetpgrp(STDOUT, SHELL_PID);
    setup_tty();

    return 0;
}
        
int set_foreground_last_updated_job() {
    job_t* job = get_job(last_updt_job);
    if (job == NULL) {
        if ((job = jobs->tail) != NULL) {
            return set_foreground_by_num(job->num);
        } else {
            send_errmsg(ALARM_FG, ALARM_NO_SUCH_JOB); 
            return -1;
        }
    } else {
        return set_foreground_by_num(last_updt_job);
    }
}

int set_background_by_num(size_t num) {
    int status;
    job_t* job = get_job(num);
    if (job == NULL) {
        send_errmsg(ALARM_BG, ALARM_NO_SUCH_JOB); 
        return -1;
    }
    
    if (job->stat == STOPPED) {
        reset_tty();
        kill(-job->pid, SIGCONT);
        job->stat = RUNNING;
        write_status(job);
    }

    tcsetpgrp(STDOUT, SHELL_PID);

    waitpid(job->pid, &status, WNOHANG | WUNTRACED);
    if (WIFEXITED(status)) {
        job->extcode = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        job->stat   = TERMINATED;
        job->signum = WTERMSIG(status);
    }

    if (WIFSTOPPED(status)) {
        job->stat = STOPPED;
        write_status(job);
        last_updt_job = job->num;
    } else {
        job->stat = DONE;
    }

    return 0;
}

int set_background_last_updated_job() {
    job_t* job = get_job(last_updt_job);
    if (job == NULL) {
        if ((job = jobs->tail) != NULL) {
            return set_background_by_num(job->num);
        } else {
            send_errmsg(ALARM_BG, ALARM_NO_SUCH_JOB); 
            return -1;
        }
    } else {
        return set_background_by_num(last_updt_job);
    }
}
