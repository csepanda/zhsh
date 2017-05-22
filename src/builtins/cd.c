static int _change_directory(char* npwd, char* opwd) {
    if (chdir(npwd)) {
        switch (errno) {
            case EACCES : alarm_msg(ALARM_CD_EACCES); break;
            case EFAULT : alarm_msg(ALARM_CD_EFAULT); break;
            case EIO    : alarm_msg(ALARM_CD_EIO   ); break;
            case ELOOP  : alarm_msg(ALARM_CD_ELOOP ); break;
            case ENOTDIR: 
            case ENOENT : alarm_msg(ALARM_CD_ENODIR); break; 
                 default: alarm_msg(ALARM_CD_DEF   ); break;    
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

    alarm_msg(ALARM_CD_DEF);
    return -1;
}
