provider darkplaces {
    probe host_loop__start(int, void *svs);
    probe time_report(char *name);
    probe sv_thread_loop__start();
    probe cmd_consolefunction__start(char *name, char *text, void *svs);
    probe cmd_consolefunction__done(char *name, char *text, void *svs);
    probe cmd_clientfunction__start(char *name, char *text, void *host_client);
    probe cmd_clientfunction__done(char *name, char *text, void *host_client);
};

provider prvm {
    probe call_builtin__start(int, void *prog);
    probe call_builtin__done(int, void *prog);
    probe enter_function(int num, char *name, char *file_name, void *prog);
    probe leave_function(int num, char *name, char *file_name, void *prog);
    probe clvm_execute_program__start(int num, char *name, char *file_name, void *prog);
    probe clvm_execute_program__done(int num, char *name, char *file_name, void *prog);
    probe svvm_execute_program__start(int num, char *name, char *file_name, void *prog);
    probe svvm_execute_program__done(int num, char *name, char *file_name, void *prog);
};
