#include "jobs.h"

static arraylist_t* jobs;
static int jobs_num = 0;
job_t* new_job(pid_t pid, char* cmd) {
    job_t* job = malloc(sizeof(job_t));
    job->pid     = pid;
    job->cmd     = cmd;
    job->num     = jobs_num++;
    job->stat    = RUNNING;
    job->extcode = 1337;
    job->signum  = 1337;
    setpgid(pid, pid);

    return job;
}

void delete_job(void* raw_job) {
    job_t* job = (job_t*) raw_job;
    free(job->cmd);
    free(job);
}

void job_init() {
    jobs = new_arraylist(255, &delete_job);
}

int add_job(pid_t pid, char* cmd) {
    job_t* job = new_job(pid, cmd);
    persist_to_arraylist(jobs, job);
    return job->num;
}

static job_t* get_job(size_t num) {
    size_t i; for (i = 0; i < jobs->size; i++) {
        job_t* job = jobs->data[i];
        if (job->num == num) {
            return job;
        }
    }

    return NULL;
}

static char* get_string_status(job_stat_t status) {
    switch (status) {
    case RUNNING:
        return "RUNNING";
    case STOPPED:
        return "STOPPED";
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
    size_t i; for (i = 0; i < jobs->size; i++) {
        job_t* job = jobs->data[i];
        write_status(job);
    }
}

int wait_jobs() {
    size_t i; for (i = 0; i < jobs->size; i++) {
        job_t* job = jobs->data[i];
        int status;
        pid_t ret = waitpid(job->pid, &status, WNOHANG);
        if (ret < 0) {
            remove_from_arraylist(jobs, i);
        } else if (ret == job->pid) {
            job->extcode = status;
            job->stat    = DONE;
            write_status(job);
            remove_from_arraylist(jobs, i);
        }
    }

    if (jobs->size == 0) {
        jobs_num = 0;
    }

    return 0;
}


int set_foreground_by_num(size_t num, int cont) {
    job_t* job = get_job(num);
    int status;
    if (job == NULL) {
        send_errmsg(ALARM_FG, ALARM_NO_SUCH_JOB); 
        return -1;
    }
    
    tcsetpgrp(STDIN, job->pid);

    if (cont) {
        kill(-job->pid, SIGCONT);
        write_status(job);
    }

    waitpid(job->pid, &status, WUNTRACED);
    if (WIFEXITED(status)) {
        job->extcode = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        job->signum = WTERMSIG(status);
    }

    if (WIFSTOPPED(status)) {
        job->stat = STOPPED;
        write_status(job);
    } else {
        job->stat = DONE;
    }



    tcsetpgrp(STDOUT, getpid());
    set_tty_ncanon();

    return 0;
}

void set_background_by_num(size_t num, int cont) {

}
