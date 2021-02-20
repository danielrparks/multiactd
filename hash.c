#include "hash.h"
#include <stdlib.h>

parent_action_t* action_hash_table[HASH_TABLE_SIZE];

static size_t action_name_hash(const char* key) {
	size_t result = 0;
	for (int i = 0; key[i]; i++)
		result ^= (size_t)(key[i]) << i * 5 % sizeof(size_t);
	return result % HASH_TABLE_SIZE;
}

parent_action_t* get_action(const char* key) {
	parent_action_t* current = action_hash_table[action_name_hash(key)];
	while (current) {
		if (!strcmp(key, current->name))
			break;
		current = current->value_next;
	}
	return current;
}

void put_action(parent_action_t* val) {
	const char* key = val->name;
	parent_action_t** toput = action_hash_table[action_name_hash(key)];
	parent_action_t* current = *toput;
	while (current) {
		if (!strcmp(key, current->name)) {
			val->value_next = current->value_next;
			free(current->name);
			free(current->command);
			free(current);
			break;
		}
		toput = &current->value_next;
		current = *toput;
	}
	*toput = val;
}
