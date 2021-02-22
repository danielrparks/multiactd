#include <getopt.h>
#include <pwd.h>
#include <stdbool.h>
#include "config.h"

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
	}
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
	for (size_t i = 0; i < num_config_files; i++) {
		parse_config(config_files[i]);
	}
	while (true) {
		// connect and fork loop (not done)
	}
}
