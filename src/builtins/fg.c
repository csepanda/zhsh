int fg_builtin(size_t argc, char** argv) {
    if (argc >= 2) {
        if (str_is_num(argv[1])) {
            size_t jnum = atol(argv[1]);
            return set_foreground_by_num(jnum, 1);
        } else {
            send_errmsg(ALARM_FG, ALARM_ILLEGAL_ARG);
            return -1;
        }
    } else {
        return set_foreground_last_updated_job();
    }
}
