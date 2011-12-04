#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern int num_errors;
extern int yyerror();
extern int yywrap();
extern int yyparse();
extern int cmdlex();
extern int count;

void c_optimize(void);
void codegen_entry(FILE *fptr);
void codegen_exit(FILE *fptr);
void find_function(void);
void print_inst(FILE*, inst_t, ddg_t *ddg);
void print_list(FILE*, inst_t, ddg_t *ddg);

inst_t instList; /* list of instructions found by parser */
int last_cycle;
int previous_type;

int main(int argc, char **argv) {
    arglim = argv + argc;
    targv = argv + 1;
    
    cmdlex();

    if (outfile == NULL)
        outfile = strdup("out.asm");

    if (infile[0]) {
        c_optimize();
    }

    return 0;
}

void c_optimize() {
    /* file pointer to dump output code */
    FILE *fptr = fopen(outfile, "w");
live_range *live;
    block_array cfg;
    ddg_t ddg;
    intNode* intGraph;

    inst_t *inst_list;
    inst_t list;
    int min_index;
    int max_index, length;
    inst_t *temp_list;
    int i;

    codegen_entry(fptr);

    yywrap();
    yyparse();

    if (num_errors > 0)
        return;

    if (verbose)
        print_list(stdout, instList, &ddg);

    find_function(); /* remove extra instructions needed for simulation */

    /************************************************************************/
    /************************************************************************/
    /************************************************************************/
    /************************************************************************/
    /*    Call your implementation from here                                */

    /* Find single basic block loops and perform Iterative Modulo Scheduling */

    //count--; // decrement counter for .opt files

    for (list = instList; list->op != OP_RET; list = list->next);
    count = list->count + 1;

    list = instList;

    inst_list = (inst_t*) malloc(count * sizeof (inst_t));

    for (i = 0; i < count; inst_list[i++] = NULL); // initialize list of instructions to NULL

    while (list) {
        inst_list[list->count] = list;
        list = list->next;
    }

    last_cycle = -1;
    previous_type = -1;
    cfg = generate_cfg();
    ddg = generate_ddg();
    exercise_ddg();
    live = liveness(count, &ddg);
    intGraph = calcInterference(live, 100);
        regAlloc(intGraph, 100, 3, instList);
    for (min_index = 0; inst_list[min_index] == NULL; min_index++);
    while (min_index < count) {
        max_index = min_index;
        while (inst_list[max_index] != NULL) {
            if (inst_list[max_index]->next != NULL) {
                if (inst_list[max_index]->next->label)
                    break;
            } else
                break;

            max_index++;
        }
        length = max_index - min_index + 1;

        temp_list = (inst_t*) malloc(length * sizeof (inst_t));
        for (i = 0; i < length; i++) {
            temp_list[i] = inst_list[min_index + i];
        }

        calc_depth(temp_list, 0, length - 1);
        free(temp_list);
        min_index = max_index + 1;
    }

    sort_by_depth(inst_list);

    ddg.ready_cycle = (int *) malloc(count * sizeof (int));
    ddg.schedule_time = (int *) malloc(count * sizeof (int));

    for (i = 0; i < count; ddg.schedule_time[i++] = -1);
    for (i = 0; i < count; ddg.ready_cycle[i++] = 0);

#ifdef MULTIOP
    for (min_index = 0; inst_list[min_index] == NULL; min_index++);
    while (min_index < count) {
        max_index = min_index;
        while (inst_list[max_index] != NULL) {
            if (inst_list[max_index]->next != NULL) {
                if (inst_list[max_index]->next->label)
                    break;
            } else
                break;

            max_index++;
        }
        length = max_index - min_index + 1;

        temp_list = (inst_t*) malloc(length * sizeof (inst_t));
        for (i = 0; i < length; i++) {
            temp_list[i] = inst_list[min_index + i];
        }

#ifdef debug
        for (i = 0; i <= length - 1; i++) {
            if (temp_list[i]->label)
                printf("%s\n", temp_list[i]->label);
            printf("%d", temp_list[i]->op);
            if (temp_list[i]->op == OP_BRA)
                printf(" branch here!\n");
            else
                printf("\n");
        }
#endif

        cycle_schedule(temp_list, &ddg, w, 0, length - 1);
        free(temp_list);
        min_index = max_index + 1;
    }
#endif

    sort_by_cycle(&ddg, inst_list);


    if (flag_regalloc) {
        // perform register allocation
        printf("Perform register allocation.\n"); // REMOVE ME
    }

    if (flag_sched) {
        // perform scheduling      
        printf("Perform scheduling.\n"); // REMOVE ME      
    }


    /************************************************************************/
    /************************************************************************/
    /************************************************************************/
    /************************************************************************/

    print_list(fptr, instList, &ddg); /* dump code to output file */

    codegen_exit(fptr);
    fclose(fptr); /* close file */
}

/**************************************************************************
 *   Support for generating code
 */

void codegen_entry(FILE *fptr) {
    fprintf(fptr, "\t.ORIG x2000\n");
    fprintf(fptr, "\tJSR main\n");
    fprintf(fptr, "\tHALT\n");
}

void codegen_exit(FILE *fptr) {
    fprintf(fptr, "\t.END\n");
}

void find_function() {
    /* Need to remove first three instructions */
    inst_t tmp, tmp1, tmp2;
    inst_t oldhead;
    int found = 0;

    if (instList->op == OP_ORIG) {
        tmp = instList->next;
        if (tmp->op == OP_JSR && strcmp(tmp->ops[0].label, "main") == 0) {
            tmp = tmp->next;
            if (tmp->op == OP_HALT) {
                found = 1;
                tmp = tmp->next;
                while (instList != tmp) {
                    oldhead = instList;
                    instList = instList->next;
                    free(oldhead);
                }
            }
        }
    }

    if (!found) {
        printf("Warning: Beginning of input not in the expected format!\n");
    }

    /* Remove last instruction: END */
    tmp = instList;
    tmp1 = tmp;
    tmp2 = tmp1;
    while (tmp != NULL) {
        tmp2 = tmp1;
        tmp1 = tmp;
        tmp = tmp->next;
    }

    if (tmp1->op == OP_END && tmp2 != tmp1) {
        tmp2->next = NULL;
        free(tmp1);
    } else {
        /*printf("Warning: Did not find .END.\n");*/
    }
}

void print_cc(FILE *fptr, int ccode) {
    if (ccode & CC_N)
        fprintf(fptr, "n");
    if (ccode & CC_Z)
        fprintf(fptr, "z");
    if (ccode & CC_P)
        fprintf(fptr, "p");

    fprintf(fptr, " ");
}

void print_op(FILE *fptr, struct operand op) {
    enum op_type t = op.t;
    switch (t) {
        case op_reg:
            fprintf(fptr, "R%d", op.reg);
            break;
        case op_imm:
            fprintf(fptr, "#%d", op.imm);
            break;
        case op_label:
            fprintf(fptr, "%s", op.label);
            break;
    }
}

void print_inst(FILE* fptr, inst_t i, ddg_t *ddg) {
#ifdef debug
    fprintf(fptr, "count = %d\t", i->count);
    fprintf(fptr, "depth = %d\t", i->depth);
#endif   

    int current_cycle = ddg->schedule_time[i->count];
#ifdef debug
    if (i->label)
        printf("%s\n", i->label);
    //printf("current cycle = %d\n", current_cycle);
    //printf("last cycle = %d\n", last_cycle);
    printf("depth = %d\n", i->depth);
#endif

#ifdef MULTIOP
    if (current_cycle == last_cycle) {
        fprintf(fptr, " . ");

        if (previous_type == OP_BR || previous_type == OP_SET)
            fprintf(fptr, "\t");
        else if (previous_type == OP_OUT || previous_type == OP_IN)
            fprintf(fptr, "\t\t");
    }
    else if (last_cycle != -1)
#endif
        fprintf(fptr, "\n");

    if (i->label) {
        fprintf(fptr, "%s:", i->label);
    }

    if (i->op == OP_BR) {
        fprintf(fptr, "\t%s", opnames[i->op]);
        print_cc(fptr, i->ccode);
    } else
        fprintf(fptr, "\t%s ", opnames[i->op]);

    switch (i->op) {

            /* 3 operands */
        case OP_ADD:
        case OP_AND:
        case OP_ANDL:
        case OP_DIV:
        case OP_LDR:
        case OP_MUL:
        case OP_OR:
        case OP_ORL:
        case OP_STR:
        case OP_SUB:
            print_op(fptr, i->ops[0]);
            fprintf(fptr, ", ");
            print_op(fptr, i->ops[1]);
            fprintf(fptr, ", ");
            print_op(fptr, i->ops[2]);
            break;
            /* 2 operands */
        case OP_BR:
        case OP_SET:
        case OP_ST:
        case OP_STI:
        case OP_LD:
        case OP_LDI:
        case OP_LEA:
        case OP_NOT:
        case OP_NOTL:
            print_op(fptr, i->ops[0]);
            fprintf(fptr, ", ");
            print_op(fptr, i->ops[1]);
            break;

            /* one operand */
        case OP_JSRR:
        case OP_BRA:
        case OP_JMP:
        case OP_JSR:
            print_op(fptr, i->ops[0]);

        default:
            break;
    }
    last_cycle = current_cycle;
    previous_type = i->op;
    //fprintf(fptr, "\n");
}

void print_list(FILE *fptr, inst_t head, ddg_t *ddg) {
    inst_t branch = NULL;
    int max_latency = 1;
    int curr_sched;
    int prev_sched = 0;

    while (head) {

        curr_sched = ddg->schedule_time[head->count];

        if (curr_sched == prev_sched)
            max_latency = max(latency(head), max_latency);
        else if (head->op == OP_RET) {
            while (max_latency != 1) {
                fprintf(fptr, "\n\tNOP");
                max_latency--;
            }
        } else {
            max_latency = latency(head);
        }

        if (head->op == OP_BRA) {
            branch = head;
            if (head->next != NULL)
                head = head->next;
        }

        if (branch) {
            if (head->next == NULL) {
                print_inst(fptr, branch, ddg);
                branch = NULL;
            } else if (head->label) {
                print_inst(fptr, branch, ddg);
                print_inst(fptr, head, ddg);
                branch = NULL;
            }
        } else
            print_inst(fptr, head, ddg);

        prev_sched = ddg->schedule_time[head->count];

        /*
                if (head->next != NULL) {
                    if (head->next->op != OP_RET)
                        max_latency = latency(head->next);
                }
         */

        head = head->next;
    }
    fprintf(fptr, "\n");
}