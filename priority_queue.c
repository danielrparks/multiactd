#include "priority_queue.h"
#include "action.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define COMPARE(x, y, z) x->time_last y z->time_last

static size_t action_queue_actual_size;
size_t action_queue_size;
parent_action_t** action_queue;

void init_queue(){
    action_queue_actual_size = 100;
    action_queue_size = 0;
    action_queue = malloc(action_queue_actual_size * sizeof(parent_action_t));
}

void resize() {
    action_queue_actual_size = action_queue_actual_size * 3 / 2 + 1;
    action_queue = realloc(action_queue, action_queue_actual_size * sizeof(parent_action_t));
}

void enqueue(parent_action_t* action) {
    if(action_queue_size >= action_queue_actual_size) {
        resize();
    }

    size_t index_to_place = action_queue_size++;
    while(index_to_place > 0 && COMPARE(action, <, action_queue[(index_to_place - 1) / 2])) { 
        action_queue[index_to_place] = action_queue[(index_to_place - 1) / 2];
        index_to_place = (index_to_place - 1) / 2;
    }
    action_queue[index_to_place] = action;
}

parent_action_t* dequeue() {
    parent_action_t* top = action_queue[0];
    size_t hole = 0;
    while(hole * 2 + 2 < action_queue_size) {
        size_t child = hole * 2 + 1;
        if(COMPARE(action_queue[child], >, action_queue[child + 1])) {
            child++;
        }
        if(COMPARE(action_queue[action_queue_size - 1], >, action_queue[child])) {
            action_queue[hole] = action_queue[child];
            hole = child;
        } else {
					break;
        }
    }
    action_queue[hole] = action_queue[action_queue_size - 1];
    action_queue_size--;
    return top;
}

#ifndef NDEBUG
void print_queue() {
    for(size_t i = 0; i < action_queue_size; i++) {
        printf("(%s %ld) ", action_queue[i]->name, action_queue[i]->time_last);
    }
    printf("\n");
}
#endif
