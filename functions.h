#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

//block_array generate_cfg();

live_range* liveness(int size);
block_array generate_cfg();
ddg_t generate_ddg();
instr_set * build_succ_list(int size);
void calculate_liveness(int size,int iterations,instr_set * succ,instr_set *livein, instr_set *liveout);
void calc_range(int size,int num_of_regs,instr_set *livein,instr_set *liveout, live_range *live);
#endif	/* FUNCTIONS_H */

