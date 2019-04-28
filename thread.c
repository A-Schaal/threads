#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>

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

tcb_t * t_halt(int terminate) {
    //get the thread at the head of thr running queue
    tcb_t * cur_tcb = (tcb_t *) running->value;
    
    if (terminate) {
        //remove this thread from the running queue
        running = remove_from_linked_list(
                running, 
                (void *) cur_tcb, 
                (compare_f) compare_tcb, 
                free
        );
       
        //we got rid of the current tcb, so no need to return it
        return NULL;

    } else {
        //remove this thread from the running queue
        running = remove_from_linked_list(
                running, 
                (void *) cur_tcb,
                (compare_f) compare_tcb, 
                free_nothing
        );
        
        //...and add it to the emd of the ready queue
        ready = append_to_linked_list(ready, (void *) cur_tcb);
        
        return cur_tcb;

        //save the location in the program of the current running thread
        //getcontext(&cur_tcb->thread_context);
    }
}

void t_run_next(start_f start) {
    //get the next tcb off of the ready queue
    tcb_t *next_tcb = (tcb_t *) ready->value;
    ready = remove_from_linked_list(
            ready, 
            (void *) next_tcb,
            (compare_f) compare_tcb, 
            free_nothing
    );
    running = append_to_linked_list(running, (void *) next_tcb);

    //if we aren't given a start function to run, switch to the last context used by this tcb
    if (NULL == start) {
        setcontext(&next_tcb->thread_context);
    //if we are given a start functin, just run it
    } else {
        start(next_tcb->thread_id);
    }
}

int t_create(start_f start, int thread_id, int priority) {
    tcb_t *new_tcb = malloc(sizeof(tcb_t));

    //intialise the fields of the tcb
    new_tcb->thread_id = thread_id;
    new_tcb->thread_priority = priority;

    int flag = 1;
    
    //halt the current thread and save its current state
    tcb_t *cur_tcb = t_halt(0);
    getcontext(&new_tcb->thread_context);

    //...then run the start function exactly once
    if (flag) {
        flag = 0;
        
        //add the new tcb to the front of the ready queue, and run it immediately
        ready = add_to_linked_list(ready, (void *) new_tcb);
        t_run_next(start);
    }
}

void t_terminate() {
    int flag = 1;
    
    //just halt the current thread amd free it in the process;
    //since we freed it, we don't need to save its current state
    t_halt(1);
    

    if (flag) {
        flag = 0;
        t_run_next(NULL);
    }
}

void t_yield() {
    int flag = 1;

    //halt the current thread and save its current state
    tcb_t *cur_tcb = t_halt(0);
    getcontext(&cur_tcb->thread_context);

    if (flag) {
        flag = 0;
        t_run_next(NULL);
    }
}
