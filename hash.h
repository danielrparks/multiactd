#ifndef MULTIACT_HASH
#define MULTIACT_HASH
#include "action.h"
#include <pthread.h>

#define HASH_TABLE_SIZE 100

extern parent_action_t* action_hash_table[HASH_TABLE_SIZE];

parent_action_t* get_action(const char* key);

void put_action(parent_action_t* val);

void clear_table();

void free_elements(parent_action_t *parent);

#endif
