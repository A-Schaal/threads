#ifndef SEMAPHORE
  #define SEMAPHORE
#include "thread.h"

  typedef struct {
    int count;
    tcb_t *q;
  } sem_t;       

  int sem_init(sem_t **sp, int sem_count);
  void sem_wait(sem_t *sp);
  void sem_signal(sem_t *sp);
  void sem_destroy(sem_t **sp);



#endif
