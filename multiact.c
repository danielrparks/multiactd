// multiact client
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <command>\n", argv[0]);
	}
	size_t len = strlen(argv[1]);
	if (len > 255) {
		fprintf(stderr, "%s: command too long\n", argv[0]);
		exit(1);
	}
	struct sockaddr_un sockaddr;
	int ret;
	int sock;
	sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (sock < 0) {
		perror(argv[0]);
		exit(1);
	}
	memset(&sockaddr, 0, sizeof(sockaddr));
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
	ret = connect(sock, (const struct sockaddr*)(&sockaddr), sizeof(sockaddr));
	if (ret) {
		perror(argv[0]);
		exit(1);
	}
	ret = write(sock, &len, 1);
	if (ret != 1) {
		perror(argv[0]);
		exit(1);
	}
	ret = write(sock, argv[1], len);
	if (ret != len) {
		perror(argv[0]);
		exit(1);
	}
	int code;
	ret = read(sock, &code, sizeof(code));
	if (ret != sizeof(code)) {
		perror(argv[0]);
		exit(1);
	}
	if (ret != 0) {
		char buf[256];
		while ((ret = read(sock, &buf, 255))) {
			write(STDERR_FILENO, buf, ret);
		}
	}
	fputc('\n', stderr);
	return code;
}
