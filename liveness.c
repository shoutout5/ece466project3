#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;

instr_set * build_succ_list(int size) {
    int i;
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
            printf("yep %d\n",succ[current->count].instr);


            instr_set *new = (instr_set *) malloc(50*sizeof (instr_set));

            succ[current->count].next = new;
            new->prev = &succ[current->count];
            new->instr = current->next->count;
            
        } else if (current->op == OP_BRA || current->op == OP_JMP) {
            printf("in br all");
            printf("here");
            //if it does then do some processing and add it to the successors list
            succ[current->count].prev = NULL;
            tmp = instList;
            //if the current operation is a BRA or JMP
            printf("here");
            succ[current->count].next = NULL;
            char *name = current->ops[0].label;
            //printf("name1: %s\n", name);
            //go through the instruction list until we find the count of the
            //instruction we're going to and add it to the successors list
            do {
                if (tmp->label != NULL && tmp->op != OP_NONE)
                    if (!strcmp(name, tmp->label)){
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
        printf("line %d: %d\n", current->count, succ[current->count].instr);
        if (current->op == OP_BR)
            printf("2nd option: %d\n", succ[current->count].next->instr);
        printf("loc = %d\n",current->count);
        if(current->count == 23)
            printf("close");
        current = current->next;
    }
}

live_range* liveness(int size, ddg_t *ddg) {

    //setup pointers walk through the lists

    /*
        inst_t tail = instList;
        inst_t tmp = instList;
    
        //find the last item in the list and assign the
        //tail pointer to  that item
        tail->prev = NULL;
        tail = tail->next;

        while (tail->next != NULL) {
            tail->prev = tmp;
            tail = tail->next;
            tmp = tmp->next;
        }
     */


    int i, iterations;
    int num_of_regs = 100; //TODO

    //create a list of liveness status for each register
    live_range *live = (live_range*) malloc(num_of_regs * sizeof (live_range));

    //create live in and live out lists for each instruction
    instr_set *livein = (instr_set *) malloc(size * sizeof (instr_set));
    instr_set *liveout = (instr_set *) malloc(size * sizeof (instr_set));

    //build successors list
    instr_set * succ = build_succ_list(size);

    //livein[i] = use[i] U (liveout[i] - def[i])
    //liveout[i] = Ux=succ(i) livein[x]
    iterations = 0;
    //we need to run three or four times to verify we have all of the items
    while (iterations < 4) {
        calculate_liveness(size, iterations, ddg, succ, livein, liveout);
    }

    //use the livein and liveout information to build a structure
    //so we can do graph coloring and register allocation
    calc_range(size, num_of_regs, livein, liveout, live);

    return live;
}

void calculate_liveness(int size, int iterations, ddg_t *ddg, instr_set * succ, instr_set *livein, instr_set *liveout) {

    instr_set *ddg_follow;
    instr_set *livein_follow;
    instr_set *liveout_follow;
    instr_set * succ_follow;
    int i, loc;
    //calculate livein and liveout by iterating backwards through the list
    for (i = size; i > 3; i--) {

        livein_follow = livein;
        liveout_follow = liveout;

        //if we are at the first instruction then do some setup
        if (i == size && iterations == 0) {
            liveout[i].next = NULL;
            liveout[i].instr = -1;
            //if there is a register used here
            if (ddg->use_inst[i].instr != 0) {
                livein[i].instr = ddg->use_inst[i].instr;
                livein_follow[i].prev = NULL;
                livein_follow[i].next = NULL;
                //if no registers are used
            } else {
                instr_set *ddg_follow = ddg->use_inst;

                while (ddg_follow[i].next != NULL) {
                    instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                    new->instr = ddg_follow[i].instr;
                    new->prev = &livein_follow[i];
                    new->next = NULL;
                    livein_follow[i] = *new;
                    ddg_follow = ddg_follow[i].next;
                }
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
            int loc = succ[i].instr;
            livein_follow = livein;
            liveout_follow = liveout;
            ddg_follow = &ddg->use_inst[i];
            livein[i].instr = -1;
            //make the use part of live in
            do {
                if (ddg_follow->instr == -1) {
                    livein[i].instr = ddg_follow->instr;
                    livein[i].instr = -1;
                    livein[i].next = NULL;
                    break;
                } else {
                    //if the first slot is empty
                    if (livein[i].instr == -1) {
                        livein[i].instr = ddg_follow->instr;
                        livein[i].next = NULL;
                        livein[i].prev = NULL;
                    } else {
                        //add a new node to the list
                        instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                        new->instr = ddg_follow->instr;
                        new->prev = &livein_follow[i];
                        new->next = NULL;
                        livein_follow[i] = *new;
                        ddg_follow = ddg_follow[i].next;
                    }
                }
            } while (ddg_follow[i].next != NULL);


            //do the (liveout[i] - def(i) part of the algorithm
            // if there is a liveout:
            if (liveout[i].instr != -1) {
                do {
                    //if the liveout is in the definition don't include it
                    if (liveout_follow[i].instr != ddg->def_inst[i].instr)
                        if (!search_ll(liveout_follow[i].instr, livein[i]))
                            if (ddg->use_inst->instr != -1) {
                                instr_set * new = (instr_set *) malloc(sizeof (instr_set));
                                new->instr = liveout_follow[i].instr;
                                new->prev = &livein_follow[i];
                                new->next = NULL;
                                livein_follow[i].next = new;
                                livein_follow[i] = *livein_follow[i].next;
                            } else {
                                printf("here i am");
                            }

                    if (liveout_follow[i].next != NULL)
                        liveout_follow[i] = *liveout_follow[i].next;
                    //if there are more to add then add them
                } while (liveout_follow[i].next != NULL);
            }

        }

    }
}

void calc_range(int size, int num_of_regs, instr_set *livein, instr_set *liveout, live_range *live) {
    size++;
}

int search_ll(int val, instr_set *curr) {
    //instr_set *curr=head;
    do {
        if (curr->instr == val)
            return 1;
    } while (curr->next != NULL);
    return 0;
}
