#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE
#include "action.h"
#include <stdint.h>

#define COMPARE_TIMESPEC(x, y, z) (x.tv_sec == z.tv_sec ? x.tv_nsec y z.tv_nsec : x.tv_nsec y x.tv_nsec)

extern size_t action_queue_size;
extern pthread_mutex_t action_queue_mutex;
extern pthread_mutexattr_t global_mutex_attrs;

void init_queue();
void enqueue(parent_action_t* action);
parent_action_t* dequeue();
parent_action_t* peek();
void action_queue_remove(parent_action_t* action);
#ifndef NDEBUG
void print_queue();
#endif

#endif
