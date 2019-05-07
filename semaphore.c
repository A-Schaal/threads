#include "semaphore.h"
#include "thread.h"
#include <stdlib.h>

int sem_init(sem_t **sp, int sem_count){
  sem_t *new_sem;
  new_sem = malloc(sizeof(sem_t));
  new_sem->count = sem_count; 
  *sp = new_sem;
}
