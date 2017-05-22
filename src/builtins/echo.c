
extern int optind; 

static void usage() {
}

int echo_builtin(size_t argc, char** argv) {
    int opt; char NO_NLN_FLAG = 0, NO_ESC_FLAG = 0;
    char buffer[4096];
    char* BSD_ECHO_FLAG = get_env("BSD_ECHO");

    while ((opt = getopt(argc, argv, "neE")) != -1) {
        switch (opt) {
            case 'n': NO_NLN_FLAG = 1;                             break;
            case 'e': NO_ESC_FLAG = BSD_ECHO_FLAG != NULL ? 0 : 1; break;
            case 'E': NO_ESC_FLAG = BSD_ECHO_FLAG != NULL ? 1 : 0; break;
            case '?':
            default:
                usage();
        }
    }
    argc -= optind;
    argv += optind;
    
    if (argc == 1) {
        write(STDOUT, argv[0], strlen(argv[0]));
    } else if (argc > 1) {
        int i = 0, j = 0;
        for (; i < argc; i++) {
            char* p = argv[i];
            while (*p) {
                buffer[j++] = *p++;
                if (j == 4093) {
                    write(0, buffer, j);
                    j = 0;
                }
            }
            buffer[j++] = i != argc - 1 ? ' ' : '\0';
            write(0, buffer, j);
            j = 0;
        }
    }

    if (!NO_NLN_FLAG) {
        write(0, "\n", 1);
    }

    return 0;
}
