#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE
#include "action.h"
#include <stdint.h>

extern size_t action_queue_size;
void init_queue();
void enqueue(parent_action_t* action);
parent_action_t* dequeue();
#ifndef NDEBUG
void print_queue();
#endif

#endif
