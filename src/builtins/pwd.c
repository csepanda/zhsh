char* _gwd() {
    size_t size = 255; char* pwd;
    while ((pwd = getcwd(NULL, size)) == NULL) {
        switch (errno) {
            case ERANGE: size = (size * 3) / 2;       continue;
            case EACCES: alarm_msg(ALARM_PWD_EACCES); NULL;
                default: alarm_msg(ALARM_PWD_DEF   ); NULL;
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

