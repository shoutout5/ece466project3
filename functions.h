#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

//block_array generate_cfg();

live_range* liveness(int size, ddg_t *ddg);
block_array generate_cfg();
ddg_t generate_ddg();
ud generate_def(int size);
ud generate_use(int size);

//register allocation
instr_set * build_succ_list(int size);
void calculate_liveness(int size, int iterations, instr_set * succ, instr_set *livein, instr_set *liveout);
instr_set *search_array_ll(int val, int size, instr_set *head);
int search_ll(int val, instr_set *head);
void calc_range(int size,int num_of_regs,instr_set *livein,instr_set *liveout, live_range *live);

intNode* calcInterference(live_range* live, int size);
void regAlloc(intNode* intGraph, int size, int k, inst_t instList);
void calc_Range(int numInstrs, int numRegs, instr_set* liveIn, 
instr_set* liveOut, live_range* live);

//scheduling
void calc_depth();
int latency(inst_t list);
int max(int first, int second);
int number_of_registers();
void cycle_schedule(inst_t *inst_list, ddg_t *ddg, int slots, int min_index, int max_index);
void sort_by_cycle(ddg_t *ddg, inst_t *inst_list);
void sort_by_depth(inst_t *inst_list);

//test benches
void exercise_ddg();
void exercise_liveness();

#endif	/* FUNCTIONS_H */

