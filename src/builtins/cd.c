static void cd_error(const char* const cause) {
    send_errmsg(ALARM_CD, cause);
}

static int _change_directory(char* npwd, char* opwd) {
    if (chdir(npwd)) {
        switch (errno) {
            case EACCES : cd_error(ALARM_PERMDENY);        break;
            case EFAULT : cd_error(ALARM_ILLEGAL_ADDRESS); break; 
            case EIO    : cd_error(ALARM_IO_FS);           break;
            case ELOOP  : cd_error(ALARM_ELOOP);           break;
            case ENOTDIR: 
            case ENOENT : cd_error(ALARM_NOSUCHDIR);       break; 
                 default: cd_error("");                    break;    
        }
        return -1;
    } 

    
    npwd = _gwd();
    set_env("PWD",    npwd);
    set_env("OLDPWD", opwd);

    return 0;
}

int cd_builtin(size_t argc, char** argv) {
    char* oldpwd = get_env("PWD");
    if (argc == 1) {
        char* path = get_env("HOME"); 
        return _change_directory(path, oldpwd);
    } else if (argc == 2) {
        return _change_directory(argv[1], oldpwd);
    } else if (argc == 3) {
        /* TODO */
        return 0;
    }

    cd_error("");
    return -1;
}
