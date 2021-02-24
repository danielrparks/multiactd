#ifndef MULTIACT_CONFIG
#define MULTIACT_CONFIG

#include <stdio.h>
#include <stdbool.h>
#include "action.h"

void parse_config(char* fname);
parent_action_t* build_parent();
child_action_t* build_child();
bool comment_line();
void readline();
void read_parent_arguments(parent_action_t* parent);
void read_children_arguments(child_action_t* child);


#endif
