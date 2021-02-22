#include "waiter.h"
#include <pthread.h>

void* waiter_thread(void* id) {
	pthread_exit(NULL);
}
