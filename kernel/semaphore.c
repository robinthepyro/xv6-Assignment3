#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/spinlock.h"
#include "kernel/semaphore.h"
#include "kernel/defs.h"

void initsema(struct semaphore* s, int count) {
  s->value = count;
  initlock(&s->lk, "Counting Semaphore");
}

int downsema(struct semaphore* s) {
  acquire(&s->lk);
  while (s->value <=0)
    sleep(s,&s->lk);
  s->value--;
  release(&s->lk);
  return s->value;
}

int upsema(struct semaphore* s) {
  acquire(&s->lk);
  s->value++;
  wakeup(s);
  release(&s->lk);
  return s->value;
}

void initrwsema(struct rwsemaphore *rws)
{
  // init reader count to 0 and init the three binary semaphores that make up a rwsemaphore
  rws->readers = 0;
  initsema(&rws->mutex, 1);
  initsema(&rws->room_empty, 1);
  initsema(&rws->turnstile, 1);
}

// A Reader enters room
int downreadsema(struct rwsemaphore *rws)
{
  // wait for turnstile, prevent writer starvation
  downsema(&rws->turnstile);
  upsema(&rws->turnstile);
  // wait for reader count mutex. (only one thread can update this at a time)
  downsema(&rws->mutex);
  //update reader count
  rws->readers += 1;
  // check if room is empty and update room_empty
  if (rws->readers == 1) {
    downsema(&rws->room_empty);
  }
  // leaving critical, increment semaphores
  upsema(&rws->mutex);

  return rws->readers;
}

// A Reader exits room
int upreadsema(struct rwsemaphore *rws)
{
  // wait for reader count mutex (Only one thread can update reader count at a time)
  downsema(&(rws->mutex));
  rws->readers--;
    if (rws->readers == 0)
    {
      upsema(&(rws->room_empty)); //Last out unlocks
    }
  // release the reader count mutex
  upsema(&(rws->mutex));

  return rws->readers;
}

// A Writer enters room
void downwritesema(struct rwsemaphore *rws)
{
  // lock the turnstile (no readers can enter now) 
  downsema(&rws->turnstile);
  // wait for room to be empty
  downsema(&rws->room_empty);
  // printf("Readers = %d\n ", rws->readers);
}

// A writer exits room
void upwritesema(struct rwsemaphore *rws)
{
  // unlock turnstile (allow readers in)
  upsema(&rws->turnstile);
  upsema(&rws->room_empty);
}

