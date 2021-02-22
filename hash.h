#ifndef MULTIACT_HASH
#define MULTIACT_HASH
#include "action.h"

#define HASH_TABLE_SIZE 100;

extern parent_action_t* action_hash_table[HASH_TABLE_SIZE];

parent_action_t* get_action(const char* key);

void put_action(const char* key, parent_action_t* val);

#endif
