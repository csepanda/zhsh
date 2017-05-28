#ifndef _ZHSH_SIGNALS_H_
#define _ZHSH_SIGNALS_H_

#include <sys/types.h>
#include <signal.h>

#define SIGCOUNT 64

typedef struct _signal {
    int   signumb;
    char* signame;
} signal_t;

signal_t get_sig(size_t num);


#endif
