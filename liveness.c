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
            //printf("in br\n");
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
            //printf("name1: %s\n", name);
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
       //printf("line %d: %d\n", current->count, succ[current->count].instr);
       // if (current->op == OP_BR)
         //   printf("2nd option: %d\n", succ[current->count].next->instr);
        current = current->next;
    }
}

live_range* liveness(int size, ddg_t *ddg) {

    //setup pointers walk through the lists
    int i, iterations;
    int num_of_regs = number_of_registers(); //TODO

    //create live in and live out lists for each instruction
    instr_set *livein = (instr_set *) malloc(size * sizeof (instr_set));
    instr_set *liveout = (instr_set *) malloc(size * sizeof (instr_set));

    //build successors list
    instr_set * succ = build_succ_list(size);

    //livein[i] = use[i] U (liveout[i] - def[i])
    //liveout[i] = Ux=succ(i) livein[x]
    iterations = 0;
    //we need to run three or four times to verify we have all of the items
    while (iterations < 1) {

        //calculate_liveness(size, iterations, num_of_regs, ddg, succ, livein, liveout);
        //printf("iter #: %d", iterations);
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

void calculate_liveness(int size, int iterations, int num_of_regs, ddg_t *ddg, instr_set * succ, instr_set *livein, instr_set *liveout) {

    instr_set *use_follow;
    instr_set *def_follow;
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
            use_follow = ddg->use_inst;

            //if there is a register used here
            instr_set *regs_used_at_curr_line = search_array_ll(i, num_of_regs, use_follow);
            livein_follow = livein;
            livein_follow[i].instr = -1;
            livein_follow[i].prev = NULL;
            do {
                if (regs_used_at_curr_line->instr != 0 && livein_follow[i].prev == NULL) {
                    livein_follow[i].instr = regs_used_at_curr_line->instr;
                    livein_follow[i].next = NULL;
                    //if this isn't the first iteration
                } else {
                    instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                    new->instr = regs_used_at_curr_line->instr;
                    new->prev = &livein_follow[i];
                    new->next = NULL;
                    livein_follow[i].next = new;
                    livein_follow[i] = *livein_follow[i].next;
                }
                regs_used_at_curr_line = regs_used_at_curr_line->next;
            } while (regs_used_at_curr_line != NULL);

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
            instr_set *regs_used_at_curr_line = search_array_ll(i, num_of_regs, use_follow);

            do {
                if (regs_used_at_curr_line->instr != 0 && livein_follow[i].prev == NULL) {
                    livein_follow[i].instr = regs_used_at_curr_line->instr;
                    livein_follow[i].next = NULL;
                    //if this isn't the first iteration
                } else {
                    instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                    new->instr = regs_used_at_curr_line->instr;
                    new->prev = &livein_follow[i];
                    new->next = NULL;
                    livein_follow[i].next = new;
                    livein_follow[i] = *livein_follow[i].next;
                }
                regs_used_at_curr_line = regs_used_at_curr_line->next;
            } while (regs_used_at_curr_line != NULL);



            //do the (liveout[i] - def(i) part of the algorithm
            // if there is a liveout:

            if (liveout[i].instr != -1) {
                //find out if a register is defined
                instr_set *reg_defed_at_curr_line = search_array_ll(i, num_of_regs, def_follow);
                do {
                    //if the liveout is in the definition don't include it
                    if (liveout_follow[i].instr != reg_defed_at_curr_line->instr)
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

void calc_range(int size, int num_of_regs, instr_set *livein, instr_set *liveout, live_range *live) {
    size++;
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

instr_set *search_array_ll(int val, int size, instr_set *head) {
    instr_set *curr=head;
    instr_set *my_list = (instr_set *) malloc(sizeof (instr_set));
    int i;
    my_list->instr = -1;

    for (i = 0; i < size; i++) {
        if(search_ll(val,&curr[i]))
            i=i+0;
            //printf("found a usage of register %d at line %d",i,val);
            
    }
        /*
        printf("first val %d",curr[i].instr);
        do {
            if (curr[i].instr == val)
                if (my_list->instr != -1) {
                    instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                    new->instr = i;
                    new->next = NULL;
                    new->prev = my_list;
                    my_list->next = new;
                    i=size;
                    break;
                } else {
                    my_list->instr = i;
                    my_list->next = NULL;
                    my_list->prev = NULL;
                }
           
            if (curr[i].next != NULL) {
                curr=curr[i].next;
            } else {
                break;
            }
        } while (curr[i].next != NULL);
        curr=head;
         
    }*/
    my_list->instr=-1;
    my_list->next=NULL;
    return my_list;
}



int *generate_def(int size){
    int i;
    inst_t curr=instList;
    int *def = (int *) malloc (size * sizeof(int *));
    for(i=0;i<size;i++){
        if(curr->op != OP_RET && curr->op != OP_BR && curr->op != OP_BRA && curr->op != OP_NOP && curr->ops[0].t == op_reg) {
            def[i]=curr->ops[0].reg;
        } else {
            def[i]=-1;
        }
        curr=curr->next;
        printf("the def at %d is %d",i,def[i]);
    }
}

int **generate_use(int size){
int **use = (int**) malloc(size * sizeof (int *));
}
