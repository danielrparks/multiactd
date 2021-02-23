#include "command.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
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
	if (COMPARE_TIMESPEC(curtm, >, parent->time_next) || parent->count > parent->num_children) {
		// wrap!
		parent->count = 0;
	}
	int exitcode = 0;
	if (parent->count == 0) {
		// operating on the parent
		ret = clock_gettime(CLOCK_MONOTONIC, &parent->time_next);
		if (ret) {
			server_exit(fd, 1, strerror(errno));
		}
		if (parent->num_children) {
			timespec_add(&parent->time_next, parent->children[0].time_limit);
		}
		if (parent->needs_wait) {
			// send to waiter thread
			pthread_mutex_lock(&action_queue_mutex);
			enqueue(parent);
			pthread_cond_broadcast(&waiter_wakeup);
			pthread_mutex_unlock(&action_queue_mutex);
		} else {
			// run the command immediately
			parent->count++;
			exitcode = execute_shell(parent->command);
		}
	} else {
		// operating on a child
		child_action_t* child = &parent->children[parent->count - 1];
		ret = clock_gettime(CLOCK_MONOTONIC, &parent->time_next);
		if (ret) {
			server_exit(fd, 1, strerror(errno));
		}
		timespec_add(&parent->time_next, child->time_limit);
		if (child->needs_wait) {
			pthread_mutex_lock(&action_queue_mutex);
			enqueue(parent);
			pthread_cond_broadcast(&waiter_wakeup);
			pthread_mutex_unlock(&action_queue_mutex);
		} else {
			parent->count++;
			exitcode = execute_shell(parent->command);
		}
	}
	pthread_mutex_unlock(&parent->lock);
	server_exit(fd, exitcode, NULL);
}
