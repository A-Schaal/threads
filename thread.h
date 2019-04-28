#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>

void t_init();
void t_shutdown();
int t_create(void (*func)(int), int thr_id, int pri);
void t_terminate();
void t_yield();

struct tcb{
  int   thread_id;
  int   thread_priority;
  ucontext_t    *thread_context;
  struct tcb    *next;
};

typedef struct tcb tcb;
