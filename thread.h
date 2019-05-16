#ifndef THREADS
#define THREADS

  #include <ucontext.h>

  #define ANY_THREAD -1
  #define NO_THREAD -2

  typedef void (*start_f) (int thread_id);

  typedef struct tcb {
    int        thread_id;
    int        thread_priority;
    ucontext_t thread_context;
  } tcb_t;

  //helper functions for using tcbs
  int compare_tcb(tcb_t *tcb_0, tcb_t *tcb_1);
  void free_tcb(tcb_t *tcb);
  
  //thread api
  void t_init();
  void t_shutdown();
  tcb_t * t_halt(int terminate, int ready);
  int t_create(start_f start, int thr_id, int pri);
  void t_run(tcb_t *next_tcb);
  void t_run_next();
  void t_terminate();
  void t_yield();
  int get_cur_thread_id();

  #define STACK_SIZE 0x10000
#endif
