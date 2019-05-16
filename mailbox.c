#include <string.h>
#include <stdlib.h>

#include "mailbox.h"
#include "linked_list.h"
#include "thread.h"
#include "semaphore.h"

int mbox_create(mbox_t **mb) {
  mbox_t *mbox = malloc(sizeof(mbox_t));

  mbox->message_queue = NULL;

  //the mbox semaphore should serve as a lock for the message queue,
  //so make it an boolean semaphore
  sem_init(&mbox->sem, 1);

  *mb = mbox;

  return 0;
}

void mbox_destroy(mbox_t **mb) {
  mbox_t *mbox = *mb;
  
  free_linked_list(mbox->message_queue, (free_f) envelope_destroy);

  sem_destroy(&mbox->sem);

  free(mbox);
}

void mbox_deposit(mbox_t *mb, char *msg, int len) {
  envelope_t *envelope;
  envelope_create(&envelope, msg, len, get_cur_thread_id(), ANY_THREAD);

  //only one thread should modify the message queue at a time
  sem_wait(mb->sem);
  mb->message_queue = append_to_linked_list(mb->message_queue, (void *) envelope);
  sem_signal(mb->sem);
}

void mbox_withdrawl(mbox_t *mb, char *msg, int *len) {
  //only one thread should modify the message queue at a time
  sem_wait(mb->sem);
  
  if (NULL != mb->message_queue) {
    //pop the queue
    envelope_t *envelope = (envelope_t *) mb->message_queue->value;
    mb->message_queue = remove_from_linked_list(
      mb->message_queue, 
      (void *) envelope,
      compare_pointer,
      free_nothing 
    );
  
  } else {
    //just return nothing if we didn't find any message in the queue
    msg = NULL;
    len = 0;
  }

  sem_signal(mb->sem);
}

int envelope_create(envelope_t **msg, char *content, int len, int sender, int receiver) {
  envelope_t *envelope = malloc(sizeof(envelope_t));

  strcpy(envelope->message, content);
  envelope->len = len;
  envelope->sender = sender;
  envelope->receiver = receiver;

  *msg = envelope;
}

void envelope_destroy(envelope_t *envelope) {
  free(envelope->message);
  free(envelope);
}
