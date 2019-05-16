#ifndef MAILBOX
  #define MAILBOX

  #include "linked_list.h"
  #include "semaphore.h"

  typedef struct envelope {
    char *message;     // copy of the message 
    int  len;          // length of the message 
    int  sender;       // TID of sender thread 
    int  receiver;     // TID of receiver thread 
  } envelope_t;

  typedef struct {
    linked_list_t *message_queue;
    sem_t         *sem;
  } mbox_t;

  //mailbox api
  int mbox_create(mbox_t **mb);
  void mbox_destroy(mbox_t **mb);
  void mbox_deposit(mbox_t *mb, char *msg, int len);
  void mbox_withdraw(mbox_t *mb, char *msg, int *len);
  void send(int tid, char *msg, int len);
  void receive(int *tid, char *msg, int *len);

  //envelope api
  int envelope_create(envelope_t **msg, char *content, int len, int sender, int receiver);
  void envelope_destroy(envelope_t envelope);
#endif
