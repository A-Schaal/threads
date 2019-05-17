#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "linked_list.h"
#include "semaphore.h"
#include "mailbox.h"

linked_list_t *running_list;
linked_list_t *ready_list;
mbox_t *post_office;

void t_init() {
  tcb_t *main = malloc(sizeof(tcb_t));

  //intialise the fields of the tcb
  main->thread_id = 0;
  main->thread_priority = 0;

  running_list = add_to_linked_list(NULL, (void *) main);
  ready_list = NULL;

  //set up our post office for holding sent messages
  mbox_create(&post_office);
}

void t_shutdown() {
  free_linked_list(running_list, (free_f) free_tcb);
  free_linked_list(ready_list, (free_f) free_tcb);

  //burn the post office TO THE GROUND...because we're done with it
  mbox_destroy(&post_office);
}

//just compares the thread ids
int compare_tcb(tcb_t *tcb_0, tcb_t *tcb_1) {
  return tcb_0->thread_id - tcb_1->thread_id;
}

void free_tcb(tcb_t *tcb) {
  //we malloc a stack pointer for every thread aside from main,
  //so it must be freed
  if (0 != tcb->thread_id) {
    free(tcb->thread_context.uc_stack.ss_sp);
  }

  free(tcb);
}

tcb_t * t_halt(int terminate, int ready) {
  if (NULL == running_list) {
    return NULL;
  }

  //get the thread at the head of thr running_list queue
  tcb_t * cur_tcb = (tcb_t *) running_list->value;
  
  //remove this thread from the running_list queue
  running_list = remove_from_linked_list(
      running_list, 
      (void *) cur_tcb,
      (compare_f) compare_tcb, 
      free_nothing      
  );

  if (terminate) {
    
    free_tcb(cur_tcb);
    return NULL;
  
  } else {
    if (ready) {
      //...and add it to the end of the ready_list queue
      ready_list = append_to_linked_list(ready_list, (void *) cur_tcb);
    }

    return cur_tcb;

    //save the location in the program of the current running_list thread
    //getcontext(&cur_tcb->thread_context);
  }
}

void t_run(tcb_t *next_tcb) {
  running_list = append_to_linked_list(running_list, (void *) next_tcb);

  setcontext(&next_tcb->thread_context);
}

void t_run_next() {
  if (NULL == ready_list) {
    return;
  }
    
  //get the next tcb off of the ready_list queue
  tcb_t *next_tcb = (tcb_t *) ready_list->value;
  ready_list = remove_from_linked_list(
      ready_list, 
      (void *) next_tcb,
      (compare_f) compare_tcb, 
      free_nothing
  );
  
  t_run(next_tcb);
}

int t_create(start_f start, int thread_id, int priority) {
  tcb_t *new_tcb = malloc(sizeof(tcb_t));

  //intialise the fields of the tcb
  new_tcb->thread_id = thread_id;
  new_tcb->thread_priority = priority;

  int flag = 1;

  getcontext(&new_tcb->thread_context);
  new_tcb->thread_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  new_tcb->thread_context.uc_stack.ss_size = STACK_SIZE;
  new_tcb->thread_context.uc_stack.ss_flags = 0;
  tcb_t *cur_tcb = (tcb_t *) running_list->value;
  new_tcb->thread_context.uc_link  = &cur_tcb->thread_context;

  if(flag){
    flag = 0;
    makecontext(&new_tcb->thread_context,  (void*)start, 1, thread_id);
    
    //add the new tcb to the front of the ready_list queue, and run it immediately
    ready_list = append_to_linked_list(ready_list, (void *) new_tcb);
  }
}

void t_terminate() {
  int flag = 1;

  //just halt the current thread amd free it in the process;
  //since we freed it, we don't need to save its current state
  t_halt(1, 0);


  if (flag) {
    flag = 0;
    t_run_next();  
  }
}

void t_yield() {
  int flag = 1;

  //halt the current thread and save its current state
  tcb_t *cur_tcb = t_halt(0, 1);
  getcontext(&cur_tcb->thread_context);

  if (flag) {
    flag = 0;
    t_run_next();
  }
}

int t_get_cur_thread_id() {
  if (NULL == running_list) {
    return NO_THREAD;
  }

  tcb_t *cur_tcb = (tcb_t *) running_list->value;
  return cur_tcb->thread_id;
}

void send(int tid, char *msg, int len) {
  envelope_t *envelope;
  envelope_create(&envelope, msg, len, t_get_cur_thread_id(), tid);

  //only one thread should modify the message queue at a time
  sem_wait(post_office->sem);
  post_office->message_queue = append_to_linked_list(post_office->message_queue, (void *) envelope);
  sem_signal(post_office->sem);
}

void receive(int *tid, char *msg, int *len) {
  //only one thread should modify the message queue at a time
  sem_wait(post_office->sem);
  
  envelope_t *envelope = (envelope_t *) find_value_in_linked_list(
    post_office->message_queue,
    (void *) tid,
    (compare_f) compare_envelope_sender
  );
  
  if (NULL == envelope) {
    printf("INSIDE NULL");
    //just return nothing if we didn't find any message in the queue
    msg = NULL;
    len = 0;
    tid = 0;
  } else {
    post_office->message_queue = remove_from_linked_list(
      post_office->message_queue, 
      (void *) envelope,
      compare_pointer,
      (free_f) free_nothing //envelope_destroy
    );
    
    strcpy(msg, envelope->message);
    *len = envelope->len;
    *tid = envelope->sender;
  }

  sem_signal(post_office->sem);
}
