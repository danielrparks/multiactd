#ifndef MULTIACT_ACTION
#define MULTIACT_ACTION

#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>

struct child_action;

typedef struct parent_action {
	char* name;
	char* command;
	struct parent_action* value_next; // the next value in the hash table (collision)
	struct child_action *children;
	size_t num_children;
	size_t count; // the number of times this action has happened within the time limit
	struct timespec time_next;
	bool needs_wait;
	bool in_queue;
	size_t time_limit;
	pthread_mutex_t lock;
} parent_action_t;

typedef struct child_action {
	char* name;
	char* command;
	parent_action_t* parent;
	size_t *undoes;
	size_t num_undos;
	size_t time_limit;
	bool needs_wait;
} child_action_t;

#endif
