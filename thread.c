#include <stdlib.h>
#include <ucontext.h>

#include "thread.h"
#include "linked_list.h"

linked_list_t *running;
linked_list_t *ready;

void t_init() {
    tcb_t *main = malloc(sizeof(tcb_t));

    //intialise the fields of the tcb
    main->thread_id = 0;
    main->thread_priority = 0;

    running = add_to_linked_list(NULL, (void *) main);
    ready = NULL;
}

void t_shutdown() {
    free_linked_list(running, free);
    free_linked_list(ready, free);
}

//just compares the thread ids
int compare_tcb(tcb_t *tcb_0, tcb_t *tcb_1) {
    return tcb_0->thread_id - tcb_1->thread_id;
}

void free_nothing(void *thing) {}

int t_create(start_f start, int thread_id, int priority) {
    tcb_t *new_tcb = malloc(sizeof(tcb_t));

    //intialise the fields of the tcb
    new_tcb->thread_id = thread_id;
    new_tcb->thread_priority = priority;

    //save this location in the program
    int flag = 1;
    getcontext(&new_tcb->thread_context);

    //...then run the start function exactly once
    if (flag) {
        flag = 0;
        start(thread_id);
    }
}

void t_switch(int terminate) {
    //only pause this thread if we have another to run
    if (NULL != ready) {
        if (terminate) {
            //remove this thread from the running queue
            running = remove_from_linked_list(running, (compare_f) compare_tcb, free);
            
        } else {
            //save the location in the program of the current running thread
            int flag = 1;
            tcb_t * cur_tcb = (tcb_t *) running->value;
            getcontext(&cur_tcb->thread_context);
        
            //remove this thread from the running queue
            running = remove_from_linked_list(running, (compare_f) compare_tcb, free_nothing);
            
            //...and add it to the emd of the ready queue
            ready = append_to_linked_list(ready, (void *) cur_tcb);
        }

        //get the next tcb off of the ready queue
        tcb_t *next_tcb = (tcb_t *) ready->value;
        ready = remove_from_linked_list(ready, (compare_f) compare_tcb, free_nothing);
        running = append_to_linked_list(running, (void *) next_tcb);

        //...then run the next thread exactly once
        if (flag) {
            flag = 0;
            setcontext(&next_tcb->thread_context);
        }
    }

}


void t_terminate() {
   t_switch(1); 
}

void t_yield() {
    t_switch(0);
}
