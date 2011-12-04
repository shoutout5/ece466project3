#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;

instr_set * build_succ_list(int size) {
    inst_t current = instList;
    inst_t tmp = instList;
    //allocate memory for the successors list
    instr_set *succ = (instr_set *) malloc(size * sizeof (instr_set));

    while (current->next != NULL) {

        if (current->op == OP_BR) {
            printf("in br\n");
            //for all other branches do the same but get all of the
            //possible labels and add them to the successors list
            succ[current->count].prev = NULL;
            tmp = instList;
            char *name = current->ops[1].label;
            do {
                if (tmp->label != NULL && tmp->op != OP_NONE)
                    if (!strcmp(name, tmp->label))
                        break;
                tmp = tmp->next;
            } while (tmp->next != NULL);

            succ[current->count].instr = tmp->count;

            instr_set *new = (instr_set *) malloc(sizeof (instr_set));

            succ[current->count].next = new;
            new->prev = &succ[current->count];
            new->instr = current->next->count;

        } else if (current->op == OP_BRA || current->op == OP_JMP) {
            //if it does then do some processing and add it to the successors list
            succ[current->count].prev = NULL;
            tmp = instList;
            //if the current operation is a BRA or JMP

            succ[current->count].next = NULL;
            char *name = current->ops[0].label;
            printf("name1: %s\n", name);
            //go through the instruction list until we find the count of the
            //instruction we're going to and add it to the successors list
            do {
                if (tmp->label != NULL && tmp->op != OP_NONE)
                    if (!strcmp(name, tmp->label)) {
                        break;
                    }
                tmp = tmp->next;
            } while (tmp->next != NULL);
            succ[current->count].instr = tmp->count;
        } else {
            //if this instruction doesn't change the PC value
            //add to the successors list
            succ[current->count].instr = current->count + 1;
            succ[current->count].next = NULL;
            succ[current->count].prev = NULL;

        }
#ifdef debug
        printf("line %d: %d\n", current->count, succ[current->count].instr);
        if (current->op == OP_BR)
            printf("2nd option: %d\n", succ[current->count].next->instr);
#endif
        current = current->next;
    }
}

live_range* liveness(int size, ddg_t *ddg) {

    //setup pointers walk through the lists
    int i, iterations;
    int num_of_regs = number_of_registers(); //TODO
    int converge = 1;
    //create live in and live out lists for each instruction
    instr_set *livein = (instr_set *) malloc(size * sizeof (instr_set));
    instr_set *liveout = (instr_set *) malloc(size * sizeof (instr_set));

    //build successors list
    instr_set * succ = build_succ_list(size);

    //livein[i] = use[i] U (liveout[i] - def[i])
    //liveout[i] = Ux=succ(i) livein[x]
    iterations = 0;
    //we need to run three or four times to verify we have all of the items
    while (iterations < converge) {

        calculate_liveness(size, iterations, succ, livein, liveout);
        //print_ll(livein);
        printf("iter #: %d", iterations);
        iterations++;
    }
    free(succ);
    //create a list of liveness status for each register
    live_range *live = (live_range*) malloc(num_of_regs * sizeof (live_range));

    //use the livein and liveout information to build a structure
    //so we can do graph coloring and register allocation
    //calc_range(size, num_of_regs, livein, liveout, live);

    return live;
}

void calculate_liveness(int size, int iterations, instr_set * succ, instr_set *livein, instr_set *liveout) {

    ud u = generate_use(size);
    ud d = generate_use(size);

    instr_set *livein_follow;
    instr_set *liveout_follow;
    instr_set * succ_follow;
    int i, loc;
    //calculate livein and liveout by iterating backwards through the list
    for (i = size; i > 3; i--) {
        // printf("current i:%d", i);
        livein_follow = livein;
        liveout_follow = liveout;

        //if we are at the first instruction then do some setup
        if (i == size && iterations == 0) {
            liveout[i].next = NULL;
            liveout[i].instr = -1;

            //if there is a register used here
            livein_follow = livein;
            livein_follow[i].instr = -1;
            livein_follow[i].prev = NULL;

            if (u.use[i][0] > 0) {
                livein_follow[i].instr = u.use[i][0];
                livein_follow[i].next = NULL;
            }
            if (u.use[i][1] > 0 && u.use[i][0] > 0) {
                instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                new->instr = u.use[i][1];
                new->prev = &livein_follow[i];
                new->next = NULL;
                livein_follow[i].next = new;
                livein_follow[i] = *livein_follow[i].next;
            }
            if (u.use[i][1] > 0 && u.use[i][0] <= 0) {
                livein_follow[i].instr = u.use[i][1];
                livein_follow[i].next = NULL;
            }


        } else {
            //we are not at the beginning so calculate liveout[i]
            //liveout[i] = Ux=succ(i) livein[x]
            succ_follow = succ;
            do {
                loc = succ_follow[i].instr;
                //do the first entry in the list
                if (livein_follow[i].instr != -1) {
                    liveout_follow[i].instr = livein_follow[loc].instr;
                    liveout_follow[i].next = NULL;
                    liveout_follow[i].prev = NULL;
                }
                //for every other entry in the list of live ins
                do {
                    if (livein_follow[loc].instr != -1) {
                        instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                        new->next = NULL;
                        new->instr = livein_follow[loc].instr;
                        new->prev = &liveout_follow[loc];
                        liveout_follow[i].next = new;
                        liveout_follow[i] = *liveout_follow[i].next;
                    }
                    livein_follow[loc] = *livein_follow[loc].next;
                } while (livein_follow[loc].next != NULL);
                succ_follow[i] = *succ_follow[i].next;
            } while (succ_follow[i].next != NULL);
            /////////////////////////////////////////////////////////////
            //livein[i] = use[i] U (liveout[i] - def[i])
            //calculate livein[i]
            livein_follow = livein;
            liveout_follow = liveout;

            livein_follow[i].prev = NULL;
            livein[i].instr = -1;

            //make the use part of live in
            if (u.use[i][0] > 0) {
                livein_follow[i].instr = u.use[i][0];
                livein_follow[i].next = NULL;
            }
            if (u.use[i][1] > 0 && u.use[i][0] > 0) {
                instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                new->instr = u.use[i][1];
                new->prev = &livein_follow[i];
                new->next = NULL;
                livein_follow[i].next = new;
                livein_follow[i] = *livein_follow[i].next;
            }
            if (u.use[i][1] > 0 && u.use[i][0] <= 0) {
                livein_follow[i].instr = u.use[i][1];
                livein_follow[i].next = NULL;
            }




            //do the (liveout[i] - def(i) part of the algorithm
            // if there is a liveout:

            if (liveout[i].instr != -1) {
                //find out if a register is defined
                do {
                    //if the liveout is in the definition don't include it
                    if (liveout_follow[i].instr != d.def[i])
                        if (!search_ll(liveout_follow[i].instr, &livein[i])) // if the livein is not in liveout
                            if (livein[i].instr != -1) {
                                instr_set * new = (instr_set *) malloc(sizeof (instr_set));
                                new->instr = liveout_follow[i].instr;
                                new->prev = &livein_follow[i];
                                new->next = NULL;
                                livein_follow[i].next = new;
                                livein_follow[i] = *livein_follow[i].next;
                            } else {
                                livein[i].instr = liveout_follow[i].instr;
                                livein[i].next = NULL;
                            }

                    if (liveout_follow[i].next != NULL)
                        liveout_follow[i] = *liveout_follow[i].next;
                    //if there are more to add then add them
                } while (liveout_follow[i].next != NULL);
            }
        }

    }
}

int search_ll(int val, instr_set *head) {
    /*instr_set *curr = head;
    int num = 1;
    do {
        if (curr->instr == val)
            return num;
        if (curr->next != NULL)
            curr = curr->next;
        else
            break;
        num++;
    } while (curr->next != NULL); */
    return 0;
}

ud generate_def(int size) {
    int i;
    inst_t curr = instList;
    int *def = (int *) malloc(size * sizeof (int *));
    for (i = 3; i < size; i++) {
        if (curr->op != OP_RET && curr->op != OP_BR && curr->op != OP_BRA && curr->op != OP_NOP && curr->ops[0].t == op_reg) {
            def[i] = curr->ops[0].reg;
        } else {
            def[i] = -1;
        }
#ifdef debug    
        printf("the def at %d is %d\n", i, def[i]);
#endif
        if (curr->next != NULL)
            curr = curr->next;
        else
            break;

    }
    ud var;
    var.def = def;
    return var;
}

ud generate_use(int size) {

    int i;
    ud var;
    int **use = (int**) malloc(size * sizeof (int *));
    for (i = 0; i < size; i++) {
        use[i] = malloc(2 * sizeof (int));
    }

    inst_t curr = instList;

    for (i = 3; i < size; i++) {
        if (curr->op != OP_RET && curr->op != OP_BRA && curr->op != OP_NOP) {
            if (curr->ops[1].t == op_reg)
                use[i][0] = curr->ops[1].reg;
            else
                use[i][0] = -1;
            if (curr->ops[2].t == op_reg)
                use[i][1] = curr->ops[2].reg;
            else
                use[i][1] = -1;
        } else if (curr->op == OP_BR) {
            if (curr->ops[0].t == op_reg)
                use[i][0] = curr->ops[0].reg;
            else
                use[i][0] = curr->ops[0].reg;
            use[i][1] = -1;
        } else {
            use[i][0] = -1;
            use[i][1] = -1;
        }
#ifdef debug   
        printf("the use at %d is %d and %d\n", i, use[i][0], use[i][1]);
#endif
        if (curr->next != NULL)
            curr = curr->next;
        else
            break;

    }
    var.use = use;
    return var;

}
