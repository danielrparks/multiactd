#include "command.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>
#include "hash.h"
#include "priority_queue.h"
#include "waiter.h"

static void server_exit(int fd, int code, char* error) {
	write(fd, &code, sizeof(code));
	if (error) {
		write(fd, error, strlen(error));
	}
	close(fd);
	exit(code);
}

void timespec_add(struct timespec* operand, const size_t milliseconds) {
	operand->tv_nsec += milliseconds * 1000000;
	int add = operand->tv_nsec >= 1000000000L;
	operand->tv_sec += add;
	operand->tv_nsec -= add ? 1000000000L : 0;
}

int execute_shell(char* cmd) {
	int err;
	int pid = fork();
	char* argv[2] = {"-c", cmd};
	if (pid < 0) {
		return errno;
	} else if (pid == 0) {
		err = execv(user_shell, argv);
		exit(err);
	} else {
		waitpid(pid, &err, 0);
		return WEXITSTATUS(err);
	}
}

static void put_in_queue(parent_action_t* parent) {
	pthread_mutex_lock(&action_queue_mutex);
	if (parent->in_queue) {
		action_queue_remove(parent);
	} else {
		parent->in_queue = true;
	}
	enqueue(parent);
	pthread_cond_broadcast(&waiter_wakeup);
	pthread_mutex_unlock(&action_queue_mutex);
}

static bool is_in_list(size_t* list, size_t size, size_t* index, size_t value) {
	if (*index >= size) return false;
	while (list[*index] < value) (*index)++;
	return list[*index] == value;
}

int dispatch_action(parent_action_t* parent) {
	// context: an action with needs_wait has just expired and now we need to run it
	// or: an action without needs_wait has been triggered
	// it could be the parent or any of the children
	int code = 0;
	assert(parent->count != 0); // concurrency issue
	size_t index = 0;
	if (parent->count == 1) { // if it's the parent action, don't worry about running anything beneath it
		code = execute_shell(parent->command);
	} else {
		child_action_t* child = &parent->children[parent->count - 2];
		if (!is_in_list(child->undoes, child->num_undos, &index, 0)) {
			// parent is not undone; dispatch parent
			code = execute_shell(parent->command);
		}
		for (size_t i = 0; i < parent->count - 2; i++) {
			child_action_t* thischild = &parent->children[i];
			if (thischild->needs_wait && !is_in_list(child->undoes, child->num_undos, &index, i + 1)) {
				// earlier child is not undone; dispatch child
				int thiscode = execute_shell(thischild->command);
				if (code == 0) code = thiscode;
			}
		}
	}
	pthread_mutex_lock(&parent->lock);
	parent->count = 0;
	pthread_mutex_unlock(&parent->lock);
	return code;
}


void do_command(int fd) {
	uint8_t cmdlen;
	int ret = read(fd, &cmdlen, 1);
	if (ret < 0) {
		server_exit(fd, 1, strerror(errno));
	}
	char* cmd = malloc(cmdlen + 1);
	ret = read(fd, cmd, cmdlen);
	if (ret != cmdlen) {
		server_exit(fd, 1, strerror(errno));
	}
	parent_action_t* parent = get_action(cmd);
	pthread_mutex_lock(&parent->lock);
	struct timespec curtm;
	ret = clock_gettime(CLOCK_MONOTONIC, &curtm);
	if (ret) {
		server_exit(fd, 1, strerror(errno));
	}
	if (COMPARE_TIMESPEC(curtm, >, parent->time_next) || parent->count > parent->num_children + 1) {
		// wrap!
		parent->count = 0;
	}
	int exitcode = 0;
	parent->count++;
	if (parent->count == 1) {
		// operating on the parent
		ret = clock_gettime(CLOCK_MONOTONIC, &parent->time_next);
		if (ret) {
			server_exit(fd, 1, strerror(errno));
		}
		if (parent->num_children) {
			timespec_add(&parent->time_next, parent->time_limit);
		}
		if (parent->needs_wait) {
			// send to waiter thread
			put_in_queue(parent);
		} else {
			// run the command immediately
			exitcode = dispatch_action(parent);
		}
	} else {
		// operating on a child
		child_action_t* child = &parent->children[parent->count - 2];
		ret = clock_gettime(CLOCK_MONOTONIC, &parent->time_next);
		if (ret) {
			server_exit(fd, 1, strerror(errno));
		}
		timespec_add(&parent->time_next, child->time_limit);
		if (child->needs_wait) {
			put_in_queue(parent);
		} else {
			exitcode = dispatch_action(parent);
		}
	}
	pthread_mutex_unlock(&parent->lock);
	server_exit(fd, exitcode, NULL);
}
