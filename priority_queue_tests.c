#include "action.h"
#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define reset "\e[0m"

void test_case();
void test_case_string();

int main() {
    printf("---TESTS---\n");
    init_queue();
    test_case(0, get_num_elements());

    // SCREW THE MEMORY LEAKS
    parent_action_t* p1 = calloc(1, sizeof(parent_action_t));
    p1->time_last = 100;
    p1->name = "p1";
    parent_action_t* p2 = calloc(1, sizeof(parent_action_t));
    p2->time_last = 200;
    p2->name = "p2";
    parent_action_t* p3 = calloc(1, sizeof(parent_action_t));
    p3->time_last = 300;
    p3->name = "p3";
    parent_action_t* p4 = calloc(1, sizeof(parent_action_t));
    p4->time_last = 400;
    p4->name = "p4";
    parent_action_t* p5 = calloc(1, sizeof(parent_action_t));
    p5->time_last = 500;
    p5->name = "p5";

    enqueue(p5);
    enqueue(p3);
    enqueue(p4);
    enqueue(p1);
    enqueue(p2);

    test_case(5, get_num_elements());

    test_case(100, deque()->time_last);
    test_case(200, deque()->time_last);
    test_case(300, deque()->time_last);
    test_case(400, deque()->time_last);
    test_case(500, deque()->time_last);

    // realloc tests
    int num_tests = 500;
    for(int i = 0; i < num_tests; i++) {
        parent_action_t *temp = calloc(1, sizeof(parent_action_t));
        temp->time_last = rand() % 100; 
        enqueue(temp);
    }

    test_case(500, get_num_elements());
    int previous = deque()->time_last;
    for (int i = 0; i < num_tests - 1; i++) {
        clock_t curr = deque()->time_last;
        if(curr < previous) {
            printf(RED "FAILED\n" reset);
        }
        previous = curr;
    }
}

void test_case(int expected, int result) {
    if (expected == result) {
        printf(GRN "PASSED TEST\n" reset);
    } else {
        printf(RED "FAILED TEST, expected %d, got %d\n" reset, expected,
               result);
    }
}

void test_case_string(char* expected, char* result) {
    if (strcmp(expected, result)) {
        printf(GRN "PASSED TEST\n" reset);
    } else {
        printf(RED "FAILED TEST, expected %s, got %s\n" reset, expected,
               result);
    }
}
