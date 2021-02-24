#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>
#include <unistd.h>
#include "config.h"
#include "waiter.h"
#include "command.h"
#include "priority_queue.h"

pthread_mutex_t action_queue_mutex;
pthread_mutexattr_t global_mutex_attrs;

int main(int argc, char* argv[]) {
	const char* short_opts = "c:d";
	struct option long_opts[] = {
		{
			.name = "config",
			.has_arg = required_argument,
			.flag = NULL,
			.val = 'c'
		},
		{
			.name = "nodaemonize",
			.has_arg = no_argument,
			.flag = NULL,
			.val = 'd'
		},
		{NULL, 0, NULL, 0}
	};
	char* config_files[] = {
		"/etc/multiact/config"
	};
	size_t num_config_files = 1;
	int opt;
	bool daemonize = true;
	while ((opt = getopt(argc, argv, short_opts)) != -1) {
		switch (opt) {
			case 'c':
				config_files[0] = strdup(optarg);
				num_config_files = 1;
				if (!config_files[0]) {
					perror(argv[0]);
					exit(1);
				}
				break;
			case 'd':
				daemonize = false;
				break;
			default:
				fprintf(stderr, "Usage: %s [-c config] [-d] command\n", argv[0]);
				break;
		}
	}
	int err;
	if (daemonize) {
		err = daemon(1, 0);
		if (err) {
			perror(argv[0]);
			exit(1);
		}
	}
	for (size_t i = 0; i < num_config_files; i++) {
		parse_config(config_files[i]);
	}
	err = pthread_mutexattr_init(&global_mutex_attrs);
	err |= pthread_mutexattr_setpshared(&global_mutex_attrs, PTHREAD_PROCESS_SHARED);
	err |= pthread_mutex_init(&action_queue_mutex, &global_mutex_attrs);
	if (err) {
		fprintf(stderr, "%s: mutexattr error", argv[0]);
		exit(1);
	}
	pthread_t waiter;
	err = pthread_create(&waiter, NULL, waiter_thread, 0);
	if (err) {
		perror(argv[0]);
		exit(1);
	}
	struct sockaddr_un sockaddr;
	int sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (sock) {
		perror(argv[0]);
		exit(1);
	}
	memset(&sockaddr, 0, sizeof(sock)); // man page said to do this
	sockaddr.sun_family = AF_UNIX;
	struct passwd* userinfo = getpwent();
	if (!userinfo) {
		perror(argv[0]);
		exit(1);
	}
	char uids[11];
	int uid_size;
	snprintf(uids, 11, "%d%n", userinfo->pw_uid, &uid_size);
	const char *sockname_begin = "/run/user/", *sockname_end = "/multiactd.sock";
	// if increasing the socket name length, ensure there is no overflow
	strcpy(sockaddr.sun_path, sockname_begin);
	strcat(sockaddr.sun_path, uids);
	strcat(sockaddr.sun_path, sockname_end);
	err = bind(sock, (const struct sockaddr*) &sockaddr, sizeof(sockaddr));
	if (err) {
		perror(argv[0]);
		exit(1);
	}
	err = listen(sock, 32);
	if (err) {
		perror(argv[0]);
		exit(1);
	}
	int data_sock, pid;
	for (;;) {
		// accept and fork loop
		data_sock = accept(sock, NULL, NULL);
		if (data_sock < 0) {
			perror(argv[0]);
			exit(1);
		}
		pid = fork();
		if (pid < 0) {
			perror(argv[0]);
			exit(1);
		} else if (pid == 0) {
			close(sock);
			do_command(data_sock);
			exit(0);
		} else {
			close(data_sock);
		}
	}
}
