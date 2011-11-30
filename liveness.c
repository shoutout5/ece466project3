#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"
#include "defines.h"

extern inst_t instList;
live_range* liveness(int size) {

//inst_t current = instList;
//inst_t head = instList;
live_range *live = (live_range*) malloc( size * sizeof(live_range));


int i;

for(i=0; i<size; i++){
	/*
        if(current->ops[0].t== op_reg){
            int v = current.ops[0].reg;
            live[v]=line_number;
        }
*/
}
return live;
}
