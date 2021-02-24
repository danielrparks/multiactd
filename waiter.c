#include <stdio.h>
#include <stdlib.h>
#include "waiter.h"
#include "priority_queue.h"
#include "command.h"

pthread_cond_t waiter_wakeup;

void* waiter_thread(void* id) {
	int ret;
	pthread_condattr_t wakeup_attrs;
	ret = pthread_condattr_init(&wakeup_attrs);
	ret |= pthread_condattr_setclock(&wakeup_attrs, CLOCK_MONOTONIC);
	ret |= pthread_condattr_setpshared(&wakeup_attrs, PTHREAD_PROCESS_SHARED);
	ret |= pthread_cond_init(&waiter_wakeup, &wakeup_attrs);
	ret |= pthread_condattr_destroy(&wakeup_attrs);
	ret |= pthread_mutex_lock(&action_queue_mutex);
	if (ret) {
		fputs("multiactd: waiter_thread: condattr", stderr);
		exit(1);
	}
	for (;;) {
		if (action_queue_size) {
			struct timespec time_now, time_next = peek()->time_next;
			ret = clock_gettime(CLOCK_MONOTONIC, &time_now);
			if (ret) {
				perror("multiactd: waiter_thread");
				exit(1);
			}
			if (COMPARE_TIMESPEC(time_now, >, time_next)) {
				dispatch_action(dequeue());
			} else {
				// wait for front of queue to elapse
				pthread_cond_timedwait(&waiter_wakeup, &action_queue_mutex, &time_next);
			}
		} else {
			pthread_cond_wait(&waiter_wakeup, &action_queue_mutex);
		}
	}
}
