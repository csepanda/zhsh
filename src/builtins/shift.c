int shift_builtin(size_t argc, char** argv) {
    size_t count = 1;
    if (argc == 2) {
        if (str_is_num(argv[1])) {
            count = atol(argv[1]);
        } else {
            send_errmsg(ALARM_SHIFT, ALARM_ILLEGAL_ARG);
            return 1;
        }
    } else if (argc > 2) {
        send_errmsg(ALARM_SHIFT, ALARM_TOO_MANY_ARGS);
        return 1;
    }
    shift_pos_args(count);
    return 0;
}
