#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE
#include "action.h"
#include <stdint.h>

typedef struct heap_node {

} heap_node_t;

// methods for the priority queue
void init_queue();
void enqueue(parent_action_t* action);
parent_action_t* deque();
int32_t compare_to(parent_action_t* first, parent_action_t* second);
void print_queue();
uint32_t get_num_elements();

#endif
