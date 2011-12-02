#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;
extern block_array cfg;
extern ddg_t ddg;

instr_set * build_succ_list(int size){
    int i;
    inst_t current = instList;
    inst_t tmp = instList;
        //allocate memory for the successors list
    instr_set *succ = (instr_set *) malloc(size * sizeof (instr_set));

    for (i = 0; i < size; i++) {
        //if this instruction doesn't change the PC value
        //add to the successors list
        if (current->op != OP_BRA && current->op != OP_BR && current->op != OP_JSRR && current->op != OP_JSR && current->op != OP_JMP) {
            succ[current->count].instr = current->count + 1;
            succ[current->count].next = NULL;
            succ[current->count].prev = NULL;
        } else {
            //if it does then do some processing and add it to the successors list
            succ[current->count].prev = NULL;
            tmp = instList;
            //if the current operation is a BRA or JMP
            if (current->op == OP_BRA || current->op == OP_JMP) {
                succ[current->count].next = NULL;
                char *name = current->ops[0].label;
                //go through the instruction list until we find the count of the
                //instruction we're going to and add it to the successors list
                while (strcmp(name, tmp->label))
                    tmp = tmp->next;
                succ[current->count].instr = tmp->count;
            } else {
                //for all other branches do the same but get all of the
                //possible labels and add them to the successors list
               char *name = current->ops[1].label;;
               
                int match=1;
                while (match){
                    if(tmp->label != NULL) 
                        if(!strcmp(name, tmp->label))
                            break;
                    tmp = tmp->next;
                }
                    
                succ[current->count].instr = tmp->count;

                instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                succ[current->count].next = new;
                new->prev = &succ[current->count];
                new->instr = current->next->count;

            }
        }
        printf("line %d: %d\n",current->count ,succ[current->count].instr );
        if(current->op == OP_BR)
            printf("2nd option: %d\n",succ[current->count].next->instr);
        current = current->next;
    }
    return succ;
}

live_range* liveness(int size) {

    //setup pointers walk through the lists
   /*
    inst_t tail = instList;
    inst_t tmp = instList;
    instr_set *ddg_follow;
    instr_set *livein_follow;
    instr_set *liveout_follow;
    int i, iterations;*/
    int num_of_regs = 100; //TODO

    //create a list of liveness status for each register
    live_range *live = (live_range*) malloc(num_of_regs * sizeof (live_range));
/*
    //create live in and live out lists for each instruction
    instr_set *livein = (instr_set *) malloc(size * sizeof (instr_set));
    instr_set *liveout = (instr_set *) malloc(size * sizeof (instr_set));


    //find the last item in the list and assign the
    //tail pointer to  that item
    tail->prev = NULL;
    tail = tail->next;

    while (tail->next != NULL) {
        tail->prev = tmp;
        tail = tail->next;
        tmp = tmp->next;
    }

    //build successors list
    

    //livein[i] = use[i] U (liveout[i] - def[i])
    //liveout[i] = Ux=succ(i) livein[x]
    iterations = 0;
    //we need to run three or four times to verify we have all of the items
    while (iterations < 4) {

        //calculate livein and liveout by iterating backwards through the list
        for (i = size; i > 0; i--) {

            livein_follow = livein;
            liveout_follow = liveout;
            //if we are at the first instruction then do some setup
            if (i == size && iterations == 0) {
                liveout[i].next = NULL;
                liveout[i].instr = -1;
                //if there is a register used here
                if (ddg.use_inst[i].instr != -1) {
                    livein[i].instr = ddg.use_inst[i].instr;
                    livein_follow[i].prev = NULL;
                    livein_follow[i].next = NULL;
                    //if no registers are used
                } else {
                    instr_set *ddg_follow = ddg.use_inst;

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
                int j = i;

                //calculate liveout[i]
                liveout_follow[i].prev = NULL;
                liveout_follow[i].next = livein_follow[j].next;
                liveout_follow[i].instr = livein_follow[j].instr;

                while (j < size) {
                    do {
                        instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                        new->next = NULL;
                        new->instr = livein_follow[j].instr;
                        new->prev = &liveout_follow[i];
                        liveout_follow[i].next = new;

                        liveout_follow[i] = *liveout_follow[i].next;
                        livein_follow[j] = *livein_follow[j].next;
                    } while (livein_follow[j].next != NULL);
                    j++;
                }


                livein[i].instr = ddg.use_inst[i].instr;
                ddg_follow = ddg.use_inst;
                livein_follow = livein;
                //make the use part of live in
                do {
                    if (ddg_follow->instr == -1)
                        break;
                    else {
                        instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                        new->instr = ddg_follow[i].instr;
                        new->prev = &livein_follow[i];
                        new->next = NULL;
                        livein_follow[i] = *new;
                        ddg_follow = ddg_follow[i].next;
                    }
                } while (ddg_follow[i].next != NULL);
                //do the (liveout[i] - def(i) part of the algorithm
                // if there is a liveout:
                if (liveout[i].instr != -1) {
                    do {
                        //if the liveout is in the definition don't include it
                        if (liveout_follow[i].instr != ddg.def_inst[i].instr) {
                            if (ddg.use_inst->instr != -1) {
                                instr_set *new = (instr_set *) malloc(sizeof (instr_set));
                                new->instr = liveout_follow[i].instr;
                                new->prev = &livein_follow[i];
                                new->next = NULL;
                                livein_follow[i].next = new;
                                livein_follow[i] = *livein_follow[i].next;
                            }
                        }

                        if (liveout_follow[i].next != NULL)
                            liveout_follow[i] = *liveout_follow[i].next;
                        //if there are more to add then add them
                    } while (liveout_follow[i].next != NULL);
                }

            }

        }
    }
    for (i = 0; i < size; i++) {


    }
*/
    return live;
}

int get_liveout() {
    int out = 0;


    return out;
}
