extern int optind; 

#define SIGCOUNT 64

typedef struct _signal {
    int   signumb;
    char* signame;
} signal_t;

signal_t signals[SIGCOUNT] = {
    {  1, "SIGHUP"	    }, {  2, "SIGINT"	    }, {  3, "SIGQUIT"	    },
    {  4, "SIGILL"	    }, {  5, "SIGTRAP"	    }, {  6, "SIGABRT"	    },
    {  7, "SIGBUS"	    }, {  8, "SIGFPE"	    }, {  9, "SIGKILL"	    },
    { 10, "SIGUSR1"	    }, { 11, "SIGSEGV"	    }, { 12, "SIGUSR2"	    },
    { 13, "SIGPIPE"	    }, { 14, "SIGALRM"	    }, { 15, "SIGTERM"	    },
    { 16, "SIGSTKFLT"	}, { 17, "SIGCHLD"	    }, { 18, "SIGCONT"	    },
    { 19, "SIGSTOP"	    }, { 20, "SIGTSTP"	    }, { 21, "SIGTTIN"	    },
    { 22, "SIGTTOU"	    }, { 23, "SIGURG"	    }, { 24, "SIGXCPU"	    },
    { 25, "SIGXFSZ"	    }, { 26, "SIGVTALRM"	}, { 27, "SIGPROF" 	    },
    { 28, "SIGWINCH"	}, { 29, "SIGIO"	    }, { 30, "SIGPWR"	    },
    { 31, "SIGSYS"	    }, { 34, "SIGRTMIN"	    }, { 35, "SIGRTMIN+1"	},
    { 36, "SIGRTMIN+2"	}, { 37, "SIGRTMIN+3"	}, { 38, "SIGRTMIN+4"	},
    { 39, "SIGRTMIN+5"	}, { 40, "SIGRTMIN+6"	}, { 41, "SIGRTMIN+7"	},
    { 42, "SIGRTMIN+8"	}, { 43, "SIGRTMIN+9"	}, { 44, "SIGRTMIN+10"	},
    { 45, "SIGRTMIN+11"	}, { 46, "SIGRTMIN+12"	}, { 47, "SIGRTMIN+13"	},
    { 48, "SIGRTMIN+14"	}, { 49, "SIGRTMIN+15"	}, { 50, "SIGRTMAX-14"	},
    { 51, "SIGRTMAX-13"	}, { 52, "SIGRTMAX-12"	}, { 53, "SIGRTMAX-11"	},
    { 54, "SIGRTMAX-10"	}, { 55, "SIGRTMAX-9"	}, { 56, "SIGRTMAX-8"	},
    { 57, "SIGRTMAX-7"	}, { 58, "SIGRTMAX-6"	}, { 59, "SIGRTMAX-5"	},
    { 60, "SIGRTMAX-4"	}, { 61, "SIGRTMAX-3"	}, { 62, "SIGRTMAX-2"	},
    { 63, "SIGRTMAX-1"	}, { 64, "SIGRTMAX"	}
};

static void kill_usage() {

}

static void print_signals() {
    int i;
    for (i = 0; i < SIGCOUNT - 1; i++) {
        signal_t sig0 = signals[i];
        signal_t sig1 = signals[++i];
        printf("%i) %s\t%i) %s\n", sig0.signumb, sig0.signame, 
                                   sig1.signumb, sig1.signame);
    }
}

int kill_builtin(size_t argc, char** argv) {
    int i, opt, signum = 15;
    char* signum_str = NULL;

    while ((opt = getopt(argc, argv, "ls:")) != -1) {
        switch (opt) {
            case 'l': print_signals(); return 0; 
            case 's': signum_str = optarg; break;
            case '?':
            default:
                kill_usage();
                return 1;
        }
    }
    
    argc -= optind;
    argv += optind;

    if (signum_str != NULL) {
        if (str_is_num(signum_str)) {
            signum = atoi(signum_str);
        } else {
            int offset = strncmp(signum_str, "SIG", 3) == 0 ? 3 : 0;
                for (i = 0; i < SIGCOUNT; i++) {
                    signal_t sig = signals[i];
                    if (strcmp(signum_str, sig.signame + offset) == 0) {
                        signum = sig.signumb;
                        break;
                    }
                }

            if (i == SIGCOUNT) {
                kill_usage();
                return 1;
            }
        }
    }

    for (i = 0; i < argc; i++) {
        pid_t pid;
        if (!str_is_num(argv[i])) {
            alarm_msg(ALARM_KILL_BAD_PID);
            return 1;
        }

        pid = atol(argv[i]);

        if (kill(pid, signum) == -1) {
            switch (errno) {
            case EINVAL: alarm_msg(ALARM_KILL_BAD_SIG); return 1;
            case EPERM:  alarm_msg(ALARM_KILL_PERM);    return 1;
            case ESRCH:  alarm_msg(ALARM_KILL_NOEXPID); break;
            }
        }
    }

    return 0;
}
