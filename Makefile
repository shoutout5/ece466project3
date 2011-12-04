
.PHONY: all tests

OBJS :=  minicsched.o s3.y.o s3.lex.o generate_cfg.o liveness.o generate_ddg.o calc_depth.o regAlloc.o coloring.o interference.o cycle_schedule.o ddg_tb.o

all: sched

sched: $(OBJS) cmdline.o
	$(LD) -m32 -o $@ $(LDFLAGS) $(LIBS) $^ 

cmdline.c:
	$(LEX) -Pcmd -o$@ cmdline.lex 

tests:
	$(MAKE) -k -C tests all

clean:
	-rm -Rf $(OBJS) cmdline.o cmdline.c
	-rm -f *.y.h *.y.output
	-rm -f sched
	-rm -f *~
	-rm -f ./tests/minic.verbose.txt

wc:
	-wc -l *.c *.y *.lex


minicopt.o:  cmdline.lex s3inst.h
cmdline.o: cmdline.c
cmdline.c: cmdline.lex
s3.y.o :s3inst.h s3.lex
liveness.o: liveness.c
generate_cfg.o: generate_cfg.c
generate_ddg.o: generate_ddg.c
calc_depth.o: calc_depth.c
interference.o: interference.c
regAlloc.o: regAlloc.c
coloring.o:coloring.c
cycle_schedule.o:cycle_schedule.c

#test benches
ddg_tb.o:ddg_tb.c

include Make.defs
include Make.rules
