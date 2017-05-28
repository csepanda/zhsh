extern int optind; 

static void kill_usage() {

}

static void print_signals() {
    int i;
    for (i = 1; i < SIGCOUNT; i++) {
        signal_t sig0 = get_sig(i);
        signal_t sig1 = get_sig(++i);
        char snb0[8];
        char snb1[8];
        char* snm0 = sig0.signame;
        char* snm1 = sig1.signame;
        size_t snbl0 = int_to_string(sig0.signumb, snb0);
        size_t snbl1 = int_to_string(sig1.signumb, snb1);

        write(STDOUT, snb0, snbl0);
        write(STDOUT, ") ", 2);
        write(STDOUT, snm0, strlen(snm0));
        write(STDOUT, "\t", 1);
        write(STDOUT, snb1, snbl1);
        write(STDOUT, ") ", 2);
        write(STDOUT, snm1, strlen(snm1));
        write(STDOUT, "\n", 1);
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
                for (i = 1; i < SIGCOUNT; i++) {
                    signal_t sig = get_sig(i);
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
            send_errmsg(ALARM_KILL, ALARM_ILLEGAL_PID);
            return 1;
        }

        pid = atol(argv[i]);

        if (kill(pid, signum) == -1) {
            switch (errno) {
            case EINVAL: send_errmsg(ALARM_KILL, ALARM_ILLEGAL_SIG);  return 1;
            case EPERM:  send_errmsg(ALARM_KILL, ALARM_PERMDENY);     return 1;
            case ESRCH:  send_errmsg(ALARM_KILL, ALARM_ILLEGAL_PROC); break;
            }
        }
    }

    return 0;
}
