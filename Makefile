
.PHONY: all tests

OBJS :=  minicsched.o s3.y.o s3.lex.o generate_cfg.o liveness.o generate_ddg.o

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

include Make.defs
include Make.rules
