

%{
  #include <string.h>
  #define MAX_IN 1000

  int verbose=0;
  char *outfile=NULL;
  int k=10;
  int w=4;

  int flag_regalloc=0;
  int flag_sched=0;
  int flag_sched_first=0;

  static int infile_cnt=0;
  char *infile[MAX_IN] = { 0 };

  void usage();
  int myinput(char *buf, int max);
#undef YY_INPUT
#define YY_INPUT(buf,result,max) (result = myinput(buf,max))
%}

%x FNAME
%x K 
%x W

%%

[ \t]*     {}

-regalloc     { flag_regalloc=1; }
-sched        { 
  if(!flag_regalloc) 
    flag_sched_first=1; 
  flag_sched=1; 
}

-h            { usage(); }
"-?"          { usage(); }

-v          { verbose = 1; }

-o          { BEGIN(FNAME); }
-k          { BEGIN(K); }
-w          { BEGIN(W); }

[^ \t]+        { infile[infile_cnt++] = strdup(yytext); }

<FNAME>[^ \t\n]+ { outfile = strdup(yytext); BEGIN(0); }
<FNAME>[ \t]+    { }

<K>[0-9]+  { k = atoi(yytext); BEGIN(0); }
<K>[ \t]+       { }

<W>[0-9]+  { w = atoi(yytext); BEGIN(0); }
<W>[ \t]+       { }


%%

void usage()
{
  printf("./sched <options> file\n"
	 "Options:                          \n"
	 "          -h,-?        : print this message\n"
         "          -v,-verbose  : print copious output to indicate state\n"
	 "          -o <file>    : output file name                      \n"
	 "          -regalloc    : perform register allocation           \n"	
	 "          -sched       : perform scheduling                    \n"
	 "          -w num       : width of target machine (4)               \n"
	 "          -k num       : number of registers for regalloc (10)    \n");	
}

char **targv;
char **arglim;

static unsigned int offset = 0;

int myinput(char *buf, int max)
{
  int len, copylen=0;

  if (targv >= arglim)
    return 0; /* EOF */

  len = strlen(*targv)-offset;
  if(len >= max)
    copylen = max-1;
  else
    copylen = len;

  if(len>0)
    memcpy(buf, targv[0]+offset, copylen);
  
  if(targv[0][offset+copylen] == '\0') {
    buf[copylen] = ' ';
    copylen++;
    offset = 0;
    targv++;
  } 

  return copylen;
}

int cmdwrap() {
  return 1;
}

