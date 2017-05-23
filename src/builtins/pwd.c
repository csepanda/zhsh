static void pwd_error(const char* const cause) {
    send_errmsg(ALARM_PWD, cause);
}

char* _gwd() {
    size_t size = 255; char* pwd;
    while ((pwd = getcwd(NULL, size)) == NULL) {
        switch (errno) {
            case ERANGE: size = (size * 3) / 2; continue;
            case EACCES: pwd_error(ALARM_PERMDENY); return NULL;
                default: pwd_error("");             return NULL;
        }
    }

    return pwd;
}
int pwd_builtin(size_t argc, char** argv) {
    char* pwd = _gwd();
    if (pwd == NULL) {
        return -1;
    }
    zhputs(pwd);
    set_env("PWD", pwd);
    return 0;
}

