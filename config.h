#ifndef MULTIACT_CONFIG
#define MULTIACT_CONFIG

#include <stdio.h>
#include <stdbool.h>
#include "action.h"

void parse_config(char* fname);
parent_action_t* build_parent(FILE* file);
child_action_t* build_child(FILE* file);
bool comment_line(char* line);

#endif
