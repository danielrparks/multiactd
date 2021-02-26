#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "action.h"
#include "config.h"
#include "hash.h"

int main() {
    // basic config
    char *filename = "config_examples/basic";
    parse_config(filename);
    parent_action_t *parent = get_action("act1");
    
    assert(parent->num_children == 3);
    assert(strcmp(parent->command, "\"Action 1 called!\"") == 0);

    // multiple parents
    filename = "config_examples/multiple_parents";
    parse_config(filename);
    parent_action_t* parent1 = get_action("act1");
    parent_action_t* parent2 = get_action("act2");
    
    assert(parent1 != NULL);
    assert(parent2 != NULL);

    assert(parent1->num_children == 3);
    assert(strcmp(parent1->command, "\"Action 1 called!\"") == 0);

    assert(parent2->num_children == 4);
    assert(strcmp(parent2->command, "\"Action 2 called!\"") == 0);

    filename = "config_examples/undo_actions";
    parse_config(filename);
    parent_action_t* parent3 = get_action("act4");
    parent_action_t* parent4 = get_action("act5");
    
    assert(parent3 != NULL);
    assert(parent4 != NULL);

    assert(parent3->num_children == 3);
    assert(strcmp(parent1->command, "\"Action 1 called!\"") == 0);
    assert(parent3->children[0].num_undos == 1);
    assert(parent3->children[0].undoes[0] == 0);
    assert(parent3->children[1].num_undos == 0);
    assert(parent3->children[2].num_undos == 2);
    assert(parent3->children[2].undoes[0] == 2);
    assert(parent3->children[2].undoes[1] == 0);

    assert(parent4->num_children == 4);
    assert(strcmp(parent2->command, "\"Action 2 called!\"") == 0);
}
