#include <stdlib.h>

#include "semaphore.h"
#include "thread.h"
#include "linked_list.h"

int sem_init(sem_t **sp, int sem_count){
  sem_t *new_sem;
  new_sem = malloc(sizeof(sem_t));
  
  //intialise fields
  new_sem->count = sem_count;
  //waitlist is initially empty
  new_sem->waitlist = NULL;
  
  //return the new semaphore at the given address
  *sp = new_sem;
}

void sem_wait(sem_t *sp) {
  //let other threads do work until we get a positive count
  while (sp->count <= 0) {
    int flag = 1;

    tcb_t *cur_tcb = t_halt(0, 0);
    getcontext(&cur_tcb->thread_context);

    if (flag) {
      flag = 0;
      sp->waitlist = append_to_linked_list(sp->waitlist, (void *) cur_tcb);
      t_run_next();
    }
  }
  
  //decrement count atomically
  __sync_sub_and_fetch(&sp->count, 1);

}

void sem_signal(sem_t *sp) {
  //increment the count atomically
  __sync_add_and_fetch(&sp->count, 1);

  //if the waitlist isn't empty...
  if (NULL != sp->waitlist) {
    int flag = 1;

    tcb_t *cur_tcb = t_halt(0,0);
    getcontext(&cur_tcb->thread_context);

    //only do this part once
    if (flag) {
      flag = 0;
      
      tcb_t *next_tcb = (tcb_t *) sp->waitlist->value;
      
      //remove from the waitlist
      sp->waitlist = remove_from_linked_list(
          sp->waitlist, 
          (void *) next_tcb, 
          (compare_f) compare_tcb,
          (free_f) free_nothing
      );

      t_run(next_tcb);
    }
  }
}

void sem_destroy(sem_t **sp) {
  free_linked_list((*sp)->waitlist, (free_f) free_nothing);

  free(*sp);
}
