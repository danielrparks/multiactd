#include "config.h"
#include "action.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// THIS FILE IS INCOMPLETE CURRENTLY
#define BUFFER_SIZE 1024

FILE* input_file;
char buffer[BUFFER_SIZE];
bool end_of_file;
const char* delimiter_characters = " \t";
char* last_token;

void parse_config(char* fname) {
    input_file = fopen(fname, "r");
    end_of_file = false;
    if (input_file == NULL) {
        fprintf(stderr, "Unable to open file %s\n", fname);
    }

    parent_action_t* parent = build_parent();

    if (ferror(input_file)) {
        perror("The following error occurred");
    }

    fclose(input_file);
}

parent_action_t* build_parent() { 
    //example:  act act1
    readline();

    strtok(buffer, delimiter_characters);
    char* parent_name = strtok(NULL, delimiter_characters);

    parent_action_t *parent = calloc(1, sizeof(parent_action_t));
    parent->name = malloc(strlen(parent_name) + 1);
    parent->name = strcpy(parent->name, parent_name);
    int num_children = 0;
    readline();
    // STRTOK MODIFIES THE STRING REMEMBER VERY CLEARLY MAKE SURE TO COPY THIS STRING NEXT TIME
    char *temp_buffer = malloc(BUFFER_SIZE);
    temp_buffer = strcpy(temp_buffer, buffer);
    char *c = strtok(temp_buffer, delimiter_characters);
    while (strcmp("next", c) != 0) {
        read_parent_arguments(parent);
        readline();
        c = strtok(temp_buffer, delimiter_characters);
    }

    parent->num_children = 0;
    parent->children = calloc(10, sizeof(child_action_t));

    while(strcmp(c, "next") == 0) {
        parent->children[num_children] = *build_child();
        num_children++;
        c = strtok(buffer, delimiter_characters);
    }
    return parent; 
}

void read_parent_arguments(parent_action_t* parent) {
    char* c = strtok(buffer, delimiter_characters);
    if (strcmp(c, "command") == 0) {
        strtok(NULL, delimiter_characters);
        // WHY IS THIS NULL WTH im calling it a day
        char *command_name = strtok(NULL, "");
        parent->command = malloc(strlen(command_name) + 1);
        parent->command = strcpy(parent->command, command_name);
    } else if (strcmp(c, "time") == 0) {
        char* time = strtok(NULL, delimiter_characters);
        parent->time_limit = atoi(time);
    }
}

void read_children_arguments(child_action_t *child) {
    char *c = strtok(buffer, delimiter_characters);
    if(strcmp(c, "command") == 0) {
        // get rid of notify-send
        strtok(NULL, delimiter_characters);
        char *command_name = strtok(NULL, "");
        child->command = malloc(strlen(command_name) + 1);
        child->command = strcpy(child->command, command_name);
    } else if(strcmp(c, "time") == 0) {
        char* time = strtok(NULL, delimiter_characters);
        child->time_limit = atoi(time);
    } 
}

child_action_t* build_child() { 
    char* first = strtok(buffer, delimiter_characters);
    child_action_t *child = calloc(1, sizeof(child_action_t));
    while(strcmp(first, "next") != 0 && !end_of_file && strcmp(first, "act") != 0) { 
        readline();
        first = strtok(buffer, delimiter_characters);
        read_children_arguments(child);
    }
    return child;
}

bool comment_line() {
    char *c = strtok(buffer, delimiter_characters);
    if(c[0] == '#') {
        return true;
    } 
    return false;
}


void readline() { 
    if(end_of_file) {
        return;
    }
    // TODO change to this later:
    /* while(comment_line()) { */
    /* end_of_file = fgets(buffer, BUFFER_SIZE, input_file) == NULL; */
    /* } */
    end_of_file = fgets(buffer, BUFFER_SIZE, input_file) == NULL;
}

/*time 1000 */
/* # begin actions */
/* act act1 */
/* 	command notify-send "Action 1 called!" */
/* 	next */
/* 		command notify-send "Action 1 called twice instead" */
/* 		time 500 */
/* 		undo 0 */
/* 	next */
/* 		command notify-send "Action 1 called a third time too" */
/* 		time 500 */
/* 	next */
/* 		command notify-send "Action 1 called four times fast" */
/* 		time 200 */
/* 		undo 2 0 */

/* act act2 */
/* 	command notify-send "Action 2 called!" */
/* 	next */
/* 		command notify-send "Action 2 called again" */
/* 	next */
/* 		command notify-send "Action 2 called again again" */
/* 	next */
/* 		command notify-send "Action 2 called again again again" */
/* 	next */
/* 		command notify-send "Action 2 called too many times" */
/* 		time 2000 */
