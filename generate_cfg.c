#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;

block_array generate_cfg() {
    block_array cfg;
    int i = -1;
    int j = 0;
    int k = -1;
    //int l;
    int quit = 0;
    inst_t line = instList;
    block *present;
    cfg.num_of_labels = 0;

    while (line != NULL) {
        if (line->label)
            cfg.num_of_labels++;

        line = line->next;
    }

    line = instList;
    cfg.label_list = malloc(cfg.num_of_labels * sizeof (block *));

    while (!line->label)
        line = line->next;

    while (quit != 1) {
        cfg.label_list[++i] = (block *) malloc(sizeof (block));
        present = cfg.label_list[i];

        while (quit != 1) {
            present->instruction = line;
            present->right = NULL;

            if (line->next != NULL) {
                present->left = (block *) malloc(sizeof (block));
                
                if (present->instruction->op == OP_BR)
                    present->right = present->left;
                
                line = line->next;
                present = present->left;
            } else
                quit = 1;

            if (line->label) {
                present->left = NULL;
                present->right = NULL;
                break;
            }
        }
    }

    while (k < i) {
        present = cfg.label_list[++k];
        
        while (!present->instruction->next->label) {
            present = present->left;
            
            if (present->instruction->op == OP_RET) {
                break;
            }
        }
///// right here
        if (present->instruction->op == OP_BRA) {     
            for (j = 0; j < cfg.num_of_labels; j++) {
                if (strcmp(cfg.label_list[j]->instruction->label, present->instruction->ops[0].label) == 0)
                   present->left = cfg.label_list[j];     
            }
        } else if (present->instruction->op == OP_BR) {
            for (j = 0; j < cfg.num_of_labels; j++) {
                if (strcmp(cfg.label_list[j]->instruction->label, present->instruction->ops[1].label) == 0)
                   present->left = cfg.label_list[j];     
            }
            //present->right = cfg.label_list[j];
        } else if (present->instruction->op == OP_RET)// RETURN STATEMENT
            break;
        else
            present->left = cfg.label_list[k+1];        // any other instruction
    }

    /*puts("CFG:");
    for (l = 0; l < cfg.num_of_labels; l++) {
        printf("label: %s\n", cfg.label_list[l]->instruction->label);
    }*/

    return cfg;
}