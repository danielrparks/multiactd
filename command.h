#ifndef MULTIACT_COMMAND
#define MULTIACT_COMMAND

extern char* user_shell;
void do_command(int fd);
int execute_shell(char* cmd);

#endif
