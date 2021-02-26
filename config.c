#include "config.h"
#include "action.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOTE TO READER this code only works on the config_examples/basic file. It
// does not work on the example config yet
#define BUFFER_SIZE 1024
#define DEFAULT_TIME 500;

FILE* input_file;
char buffer[BUFFER_SIZE];
bool end_of_file;
const char* delimiter_characters = " \t\n";

// TODO deal with undoes
// TODO deal with syntax errors
// TODO deal with multiple parent actions (while loop in parse_config)
// TODO deal with an default time if there's one at the top of the file
// TODO set a default time if there's on default time at the top

void parse_config(char* fname) {
    input_file = fopen(fname, "r");
    end_of_file = false;
    if (input_file == NULL) {
        fprintf(stderr, "Unable to open file %s\n", fname);
    }

    readline();
    while (!end_of_file) {
        parent_action_t* parent = build_parent();
        put_action(parent);
    }

    if (ferror(input_file)) {
        perror("The following error occurred");
    }

    fclose(input_file);
}

parent_action_t* build_parent() {
    // example:  act act1
    strtok(buffer, delimiter_characters);
    char* parent_name = strtok(NULL, delimiter_characters);

    parent_action_t* parent = calloc(1, sizeof(parent_action_t));
    parent->name = malloc(strlen(parent_name) + 1);
    parent->name = strcpy(parent->name, parent_name);
    parent->time_limit = DEFAULT_TIME;
    readline();

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

    // the act part is redundant i suppose
    while (strcmp(first, "next") == 0 && !end_of_file && strcmp(first, "act") != 0) {
        parent->children[num_children] = *build_child();
        num_children++;
        // resize if there's too many children
        if (num_children == max_num_children) {
            max_num_children *= 2;
            parent->children = realloc(
                parent->children, max_num_children * sizeof(child_action_t));
        }
        first = get_first_word();
    }
    parent->num_children = num_children;
    return parent;
}

child_action_t* build_child() {
    child_action_t* child = calloc(1, sizeof(child_action_t));
    readline();
    char* first = get_first_word();
    child->time_limit = DEFAULT_TIME;

    while (strcmp(first, "next") != 0 && !end_of_file &&
           strcmp(first, "act") != 0) {
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
        char* command_name = strtok(NULL, "\n");
        parent->command = malloc(strlen(command_name) + 1);
        parent->command = strcpy(parent->command, command_name);
    } else if (strcmp(descriptor, "time") == 0) {
        char* time = strtok(NULL, delimiter_characters);
        parent->time_limit = atoi(time);
    }
}

void read_children_arguments(child_action_t* child) {
    char* descriptor = strtok(buffer, delimiter_characters);
    if (strcmp(descriptor, "command") == 0) {
        // get rid of notify-send
        strtok(NULL, delimiter_characters);
        char* command_name = strtok(NULL, "");
        child->command = malloc(strlen(command_name) + 1);
        child->command = strcpy(child->command, command_name);
    } else if (strcmp(descriptor, "time") == 0) {
        char* time = strtok(NULL, delimiter_characters);
        child->time_limit = atoi(time);
    }
}

// TODO make sure you copy the buffer like you did in first word
bool ignore_line() {
    char* first = get_first_word();
    // we want to ignore commented lines and blank lines
    if (first == NULL || first[0] == '#') {
        return true;
    }
    free(first);
    return false;
}

char* get_first_word() {
    char* temp_buffer = malloc(BUFFER_SIZE + 1);
    temp_buffer = strcpy(temp_buffer, buffer);
    char* c = strtok(temp_buffer, delimiter_characters);
    if(c == NULL) { 
        return NULL;
    }
    char* first = malloc(strlen(c) + 1);
    first = strcpy(first, c);
    return first;
}

void readline() {
    if(end_of_file) {
        return;
    }
    end_of_file = fgets(buffer, BUFFER_SIZE, input_file) == NULL;
    // ignore commented lines and blank lines
    while (ignore_line() && !end_of_file) {
        end_of_file = fgets(buffer, BUFFER_SIZE, input_file) == NULL;
    }
}
