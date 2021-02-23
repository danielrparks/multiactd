#include <stdio.h>
#include <stdlib.h>
#include "waiter.h"
#include "priority_queue.h"
#include "command.h"

pthread_cond_t waiter_wakeup = PTHREAD_COND_INITIALIZER;

static bool is_in_list(size_t* list, size_t size, size_t* index, size_t value) {
	if (*index >= size) return false;
	while (list[*index] < value) (*index)++;
	return list[*index] == value;
}

static void process_action(parent_action_t* parent) {
	// context: an action with needs_wait has just expired and now we need to run it
	// it could be the parent or any of the children
	size_t index = 0;
	child_action_t* child = &parent->children[parent->count - 1];
	if (!is_in_list(child->undoes, child->num_undos, &index, 0)) {
		// parent is not undone; dispatch parent
		execute_shell(parent->command);
	}
	for (size_t i = 1; i < parent->count; i++) {
		child_action_t* thischild = &parent->children[i - 1];
		if (thischild->needs_wait && !is_in_list(child->undoes, child->num_undos, &index, i)) {
			// earlier child is not undone; dispatch child
			execute_shell(thischild->command);
		}
	}
}

void* waiter_thread(void* id) {
	pthread_mutex_lock(&action_queue_mutex);
	int ret;
	for (;;) {
		if (action_queue_size) {
			struct timespec time_now, time_next = peek()->time_next;
			ret = clock_gettime(CLOCK_MONOTONIC, &time_now);
			if (ret) {
				perror("multiactd: waiter_thread");
				exit(1);
			}
			if (COMPARE_TIMESPEC(time_now, >, time_next)) {
				process_action(dequeue());
			} else {
				// wait for front of queue to elapse
				pthread_cond_timedwait(&waiter_wakeup, &action_queue_mutex, &time_next);
			}
		} else {
			pthread_cond_wait(&waiter_wakeup, &action_queue_mutex);
		}
	}
}
