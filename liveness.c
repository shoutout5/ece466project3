#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;
extern block_array cfg;
extern ddg_t ddg;

live_range* liveness(int size) {

inst_t current = instList;
inst_t tail = instList;
inst_t tmp = instList;
live_range *live = (live_range*) malloc( size * sizeof(live_range));

tail->prev=NULL;
tail=tail->next;

while(tail->next != NULL) {
    tail->prev=tmp;
    tail=tail->next;
    tmp=tmp->next;
}

//build successors list
instr_set *succ = (instr_set *) malloc(size * sizeof (instr_set));
int i;

for(i=0; i<size; i++) {
    
    if(current->op != OP_BRA && current->op != OP_BR && current->op != OP_JSRR && current->op != OP_JSR && current->op != OP_JMP){
        succ[current->count].instr=current->count+1;
        succ[current->count].next=NULL;
        succ[current->count].prev=NULL;
    }
    else{
        succ[current->count].prev=NULL;
        tmp = instList;
        if(current->op == OP_BRA || current->op == OP_JMP){
            succ[current->count].next=NULL;
            char *name=current->ops[0].label;
            while(strcmp(name,tmp->label))
                tmp=tmp->next;
            succ[current->count].instr=tmp->count;
        }
        else {
            char *name=current->ops[1].label;
            while(strcmp(name,tmp->label))
                tmp=tmp->next;
            succ[current->count].instr=tmp->count;

            instr_set *new = (instr_set *) malloc(sizeof (instr_set));
            succ[current->count].next=new;
            new->prev=&succ[current->count];
            new->instr=current->next->count;
            
        }
    }
    printf("%d",succ[current->count].instr);
    current=current->next;
}

//calculate livein
//livein[i] = use[i] U (liveout[i] - def[i])


//liveout[i] = Ux=succ(i) livein[x]

for(i=size; i>0; i--){
    if(i==size){
        
    }

}


return live;
}
