// Generic Counting Semaphore
struct semaphore {
  int value;
  struct spinlock lk;
};

// Prototype of the three semaphore functions in semaphore.c
void initsema(struct semaphore*, int);
int downsema(struct semaphore*);
int upsema(struct semaphore*);


// Read/Write Semaphore
struct rwsemaphore {
  int readers;    
  struct semaphore mutex;
  struct semaphore room_empty;
  struct semaphore turnstile;
};

void initrwsema(struct rwsemaphore *);
int downreadsema(struct rwsemaphore *);
int upreadsema(struct rwsemaphore *);
void downwritesema(struct rwsemaphore *);
void upwritesema(struct rwsemaphore *);
