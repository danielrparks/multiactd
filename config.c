#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "action.h"
#include "config.h"
#include "hash.h"

// THIS FILE IS INCOMPLETE CURRENTLY
void parse_config(char* fname) {

    FILE *fptr = fopen(fname, "r");
    if(fptr == NULL) {
        perror("File failed to open");
        return;
    }
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fptr)) != -1) {
        /* printf("Retrieved line of length %zu:\n", read); */
        printf("%s", line);
    }

    fclose(fptr);
    if (line)
        free(line);
}

parent_action_t* build_parent(FILE* fptr) {

        return NULL;
}

child_action_t* build_child(FILE *fptr) {
    return NULL;
}

bool comment_line(char* line) {
    return false;
}


/*
outline:

   */

/* # sets default time */
/* time 1000 */

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
