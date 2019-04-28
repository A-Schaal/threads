#include "thread.h"

ucontext_t *running;
ucontext_t *ready;

void t_init(){
  ucontext_t *tmp;
  tmp = (ucontext_t *) malloc(sizeof(ucontext_t));
  getcontext(tmp); 
  running = tmp;
}

int t_create(void(*fct)(int), int id, int pri){
  size_t sz = 0x10000;
  ucontext_t *uc;
  uc = (ucontext_t *) malloc(sizeof(ucontext_t));
  getcontext(uc);

  uc->uc_stack.ss_sp = malloc(sz);
  uc->uc_stack.ss_size = sz;
  uc->uc_stack.ss_flags = 0;
  uc->uc_link = running;
  makecontext(uc, fct, 1, id);
  ready = uc;
}
