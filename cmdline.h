#ifndef CMDLINE_H
#define CMDLINE_H

extern char **targv, **arglim;
extern char *outfile;
extern char *infile[];
extern int verbose;
extern int k;
extern int w;
char * get_filename();
int    get_lineno();

extern  int flag_regalloc;
extern  int flag_sched;
extern  int flag_sched_first;


#endif
