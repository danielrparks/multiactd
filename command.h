#ifndef MULTIACT_COMMAND
#define MULTIACT_COMMAND
#include "action.h"

extern char* user_shell;
void do_command(int fd);
int execute_shell(char* cmd);
int dispatch_action(parent_action_t* parent);

#endif
