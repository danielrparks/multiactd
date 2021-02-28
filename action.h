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
	char* undo_command;
	struct parent_action* value_next; // the next value in the hash table (collision)
	struct child_action *children;
	size_t num_children;
	size_t count; // the number of times this action has happened within the time limit
	size_t time_limit;
	struct timespec time_next;
	bool in_queue;
	pthread_mutex_t lock;
} parent_action_t;

typedef struct child_action {
	char* name;
	char* command;
	char* undo_command;
	size_t time_limit;
} child_action_t;

#endif
