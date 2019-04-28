#ifndef THREADS
    #define THREADS

    #include <ucontext.h>

    typedef void (*start_f) (int thread_id);

    typedef struct tcb {
        int        thread_id;
        int        thread_priority;
        ucontext_t thread_context;
    } tcb_t;

    void t_init();
    void t_shutdown();
    int t_create(start_f start, int thr_id, int pri);
    void t_terminate();
    void t_yield();
#endif
