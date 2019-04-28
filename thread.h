#ifndef THREADS
    #define THREADS

    typedef void (*start_f) (int);

    typedef struct tcb {
        int         thread_id;
        int         thread_priority;
        ucontext_t *thread_context;
    } tcb_t;

    void t_init();
    void t_shutdown();
    int t_create(void (*func)(int), int thr_id, int pri);
    void t_terminate();
    void t_yield();
#endif
