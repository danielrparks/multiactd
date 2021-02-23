#include "command.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "hash.h"

static void server_exit(int fd, int code, char* error) {
	write(fd, &code, sizeof(code));
	write(fd, error, strlen(error));
	exit(code);
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
	if (parent->count == 0) {
		// operating on the parent
		if (parent->needs_wait) {
			// send to waiter thread
		} else {

		}
	}
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
