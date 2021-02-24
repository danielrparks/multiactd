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
const char* delimiter_characters = " \t\n";

//TODO deal with undoes
//TODO deal with syntax errors
//TODO deal with multiple parent actions (while loop in parse_config)
//TODO deal with blank lines and commented lines
//TODO deal with an default time if there's one at the top of the file
//TODO set a default time if there's on default time at the top

void parse_config(char* fname) {
    input_file = fopen(fname, "r");
    end_of_file = false;
    if (input_file == NULL) {
        fprintf(stderr, "Unable to open file %s\n", fname);
    }

    parent_action_t* parent = build_parent();
    put_action(parent);
    

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
    readline();
    // STRTOK MODIFIES THE STRING REMEMBER VERY CLEARLY MAKE SURE TO COPY THIS STRING NEXT TIME
    char *temp_buffer = malloc(BUFFER_SIZE + 1);
    char* first = get_first_word();
    while (strcmp("next", first) != 0) {
        read_parent_arguments(parent);
        readline();
        free(first);
        first = get_first_word();
    }

    int num_children = 0;
    int max_num_children = 10;
    parent->children = calloc(max_num_children, sizeof(child_action_t));

    while(strcmp(first, "next") == 0 && !end_of_file && strcmp(first, "act")) {
        parent->children[num_children] = *build_child();
        num_children++;
        first = get_first_word();
    }
    parent->num_children = num_children;

    return parent; 
}


child_action_t* build_child() { 
    child_action_t *child = calloc(1, sizeof(child_action_t));
    readline();
    char* first = get_first_word();

    while(strcmp(first, "next") != 0 && !end_of_file && strcmp(first, "act") != 0) { 
        read_children_arguments(child);
        readline();
        free(first);
        first = get_first_word();
    }
    return child;
}

void read_parent_arguments(parent_action_t* parent) {
    char* descriptor = strtok(buffer, delimiter_characters);
    if (strcmp(descriptor, "command") == 0) {
        strtok(NULL, delimiter_characters);
        char *command_name = strtok(NULL, "");
        parent->command = malloc(strlen(command_name) + 1);
        parent->command = strcpy(parent->command, command_name);
    } else if (strcmp(descriptor, "time") == 0) {
        char* time = strtok(NULL, delimiter_characters);
        parent->time_limit = atoi(time);
    }
}

void read_children_arguments(child_action_t *child) {
    char *descriptor = strtok(buffer, delimiter_characters);
    if(strcmp(descriptor, "command") == 0) {
        // get rid of notify-send
        strtok(NULL, delimiter_characters);
        char *command_name = strtok(NULL, "");
        child->command = malloc(strlen(command_name) + 1);
        child->command = strcpy(child->command, command_name);
    } else if(strcmp(descriptor, "time") == 0) {
        char* time = strtok(NULL, delimiter_characters);
        child->time_limit = atoi(time);
    } 
}

//TODO make sure you copy the buffer like you did in first word
bool comment_line() {
    char *c = strtok(buffer, delimiter_characters);
    if(c[0] == '#') {
        return true;
    } 
    return false;
}

char* get_first_word() {
    char *temp_buffer = malloc(BUFFER_SIZE + 1);
    temp_buffer = strcpy(temp_buffer, buffer);
    char* c = strtok(temp_buffer, delimiter_characters);
    char* first = malloc(strlen(c) + 1);
    first = strcpy(first, c);
    return first;
}

void readline() { 
    if(end_of_file) {
        return;
    }
    // TODO change to this later, make sure you change comment_line to work
    /* while(comment_line()) { */
    /* end_of_file = fgets(buffer, BUFFER_SIZE, input_file) == NULL; */
    /* } */
    end_of_file = fgets(buffer, BUFFER_SIZE, input_file) == NULL;
}
