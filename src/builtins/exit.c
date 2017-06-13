int exit_builtin(size_t argc, char** argv) {
    int exit_code;
    if (argc > 2) {
        send_errmsg(ALARM_EXIT, ALARM_TOO_MANY_ARGS);
        return 1;
    }

    if (argc == 2 && str_is_num(argv[1])) {
        exit_code = atoi(argv[1]);
    } else {
        exit_code = 0;
    }

    reset_tty();
    history_save();
    _exit(exit_code);

    return 0;
}
