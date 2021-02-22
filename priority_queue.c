#include "priority_queue.h"
#include "action.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

uint32_t size;
uint32_t num_elements;
parent_action_t** action_queue;

void init_queue(){
    size = 100;
    num_elements = 0;
    action_queue = malloc(size * sizeof(parent_action_t));
}

void resize() {
    size *= 2 + 1;
    action_queue = realloc(action_queue, size * sizeof(parent_action_t));
}

void enqueue(parent_action_t* action) {
    if(num_elements > size - 1) {
        resize();
    }

    num_elements++;
    uint32_t index_to_place = num_elements;
    while(index_to_place > 1 && compare_to(action, action_queue[index_to_place / 2]) < 0) { 
        action_queue[index_to_place] = action_queue[index_to_place/2];
        index_to_place /= 2;
    }
    action_queue[index_to_place] = action;
}

parent_action_t* deque() {
    parent_action_t* top = action_queue[1];
    int hole = 1;
    bool done = false;
    while(hole * 2 < num_elements && !done) {
        int child = hole * 2;
        if(compare_to(action_queue[child], action_queue[child + 1]) > 0) {
            child++;
        }
        if(compare_to(action_queue[num_elements], action_queue[child]) > 0) {
            action_queue[hole] = action_queue[child];
            hole = child;
        } else {
            done = true;
        }
    }
    action_queue[hole] = action_queue[num_elements];
    num_elements--;
    return top;
}

int compare_to(parent_action_t *first, parent_action_t *second) {
    return first->time_last - second->time_last;
}

void print_queue() {
    for(int i = 1; i <= num_elements; i++) {
        printf("(%s %ld) ", action_queue[i]->name, action_queue[i]->time_last);
    }
    printf("\n");
}

uint32_t get_num_elements() {
    return num_elements;
}
