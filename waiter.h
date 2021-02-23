#ifndef MULTIACT_WAITER
#define MULTIACT_WAITER
#include <pthread.h>

extern pthread_cond_t waiter_wakeup;

void* waiter_thread(void* id);

#endif
