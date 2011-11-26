%{
#include "s3inst.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmdline.h"

extern int yylex();   
int yyerror();
int yywrap();

int num_errors=0;
extern FILE * yyin;
extern int line_num;

inst_t new_inst(inst_t);
 inst_t append_inst(inst_t, inst_t);

extern inst_t instList;
 void print_inst(inst_t);
%}

%union {
  char * label;
  char * string;
  int    reg;
  int    pred;
  int    imm;
  int    nzp;
  struct inst_d inst;
  inst_t inst_ptr;
}

%token INST_SEP ENDLINE LPAREN RPAREN COMMA EQ

/* branch symbols */
%token BR_CC

/* predication symbols */
%token PCODE_NE PCODE_GT PCODE_GE PCODE_EQ PCODE_LE PCODE_LT 
%token PTYPE_AND PTYPE_OR PTYPE_UNC

 /* instructions */
%token ADD AND ANDL BR BRA DIV JMP JSRR JSR LDI LDR LD LEA MUL NOP NOT NOTL OR ORL
%token RTI RET SET STI STR ST SUB TRAP

/* macros  */
%token HALT IN OUT GETC PUTS PUTSP ORIG END

/* operands */ 
%token REGISTER LABEL IMM PREDICATE STRING

%type <reg> REGISTER
%type <label> LABEL
%type <string> STRING
%type <pred> PREDICATE

%type <inst> inst ADD ANDL BR BRA DIV JMP JSRR JSR LDI LDR LD LEA MUL NOP NOT NOTL OR ORL
%type <inst> RTI RET SET STI STR ST SUB TRAP
%type <inst> HALT IN OUT GETC PUTS PUTSP 

%type <inst_ptr> inst_list inst_p 

%type <imm>  IMM
%type <label> label_opt
%type <nzp>  BR_CC
 

%%

inst_list :   inst_list inst_p 
{ 
  $$ = append_inst($1,$2);
  instList = $$;
}
| inst_p 
{ 
  $$ = $1;
}
;


inst_p    :    label_opt inst ENDLINE
{
  $$ = new_inst(&($2));
  ($$)->label = $1;
  ($$)->endOp = 1;
  $$->next = NULL;
}
             | label_opt inst INST_SEP
{
  $$ = new_inst(&($2));
  ($$)->label = $1;
  ($$)->endOp = 0; 
  $$->next = NULL;
}
             | label_opt ENDLINE
{
  inst_t tmp = (inst_t) malloc( sizeof(*tmp) );
  tmp->op = OP_NOP;
  tmp->label = $1;
  tmp->ccode = 0;
  tmp->data = 0;
  tmp->endOp = 1;
  tmp->next = NULL;
  $$ = tmp;
}
;

label_opt  : { $$ = NULL; }  
| LABEL { $$ = $1; }
;

inst      :   
               ADD REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_ADD;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_REG($6);
}
             | ADD REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_ADD;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_IMM($6);
}

             | AND REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_AND;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_REG($6);
}
             | AND REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_AND;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_IMM($6);
}

             | ANDL REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_ANDL;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_REG($6);
}
             | ANDL REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_ANDL;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_IMM($6);
}

	     | BR REGISTER COMMA LABEL
{
  $$.op = OP_BR;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_LABEL($4);
}
             | BR BR_CC REGISTER COMMA LABEL
{
  $$.op = OP_BR;
  $$.ccode = $2;
  $$.ops[0] = OP_REG($3);
  $$.ops[1] = OP_LABEL($5);
}

	     | BRA LABEL
{
  $$.op = OP_BRA;
  $$.ops[0] = OP_LABEL($2);
}
             | DIV REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_DIV;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_REG($6);
}
             | DIV REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_DIV;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_IMM($6);
}
	     | JSR LABEL
{
  $$.op = OP_JSR;
  $$.ops[0] = OP_LABEL($2);
}
	     | JSRR REGISTER
{
  $$.op = OP_JSRR;
  $$.ops[0] = OP_REG($2);
}

	     | LDI REGISTER COMMA LABEL
{
  $$.op = OP_LDI;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_LABEL($4);
}
	     | LDI REGISTER COMMA IMM
{
  $$.op = OP_LDI;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_IMM($4);
}
             | LDR REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_LDR;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_REG($6);
}
             | LDR REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_LDR;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_IMM($6);
}
	     | LD REGISTER COMMA LABEL
{
  $$.op = OP_LD;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_LABEL($4);
}
	     | LD REGISTER COMMA IMM
{
  $$.op = OP_LD;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_IMM($4);
}
	     | LEA REGISTER COMMA LABEL
{
  $$.op = OP_LEA;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_LABEL($4);
}
	     | LEA REGISTER COMMA IMM
{
  $$.op = OP_LEA;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_IMM($4);
}	    
             | MUL REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_MUL;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_REG($6);
}
             | MUL REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_MUL;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);
  $$.ops[2] = OP_IMM($6);
}
	     | NOP
{
  $$.op = OP_NOP;
}
	     | NOT REGISTER COMMA REGISTER
{
  $$.op = OP_NOT;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);  
}
	     | NOTL REGISTER COMMA REGISTER
{
  $$.op = OP_NOTL;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);  
}

             | OR REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_OR;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_REG($6);  
}
             | OR REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_OR;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_IMM($6);  
}

             | ORL REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_ORL;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_REG($6);  
}
             | ORL REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_ORL;
  $$.ops[0] = OP_REG($2);
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_IMM($6);  
}
             | RET
{
  $$.op = OP_RET;
  $$.ops[0] = OP_REG(7);
}
      
	     | SET REGISTER COMMA IMM
{
  $$.op = OP_SET;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_IMM($4);  
}
	     
	     | STI REGISTER COMMA LABEL
{
  $$.op = OP_STI;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_LABEL($4);  
}
	     | STI REGISTER COMMA IMM
{
  $$.op = OP_STI;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_IMM($4);  
}
             | STR REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_STR;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_REG($6);  
}
             | STR REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_STR;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_IMM($6);  
}


	     | ST REGISTER COMMA LABEL
{
  $$.op = OP_STR;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_LABEL($4);  
}
	     | ST REGISTER COMMA IMM
{
  $$.op = OP_ST;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_IMM($4);  
}
             | SUB REGISTER COMMA REGISTER COMMA REGISTER
{
  $$.op = OP_SUB;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_REG($6);  
}
             | SUB REGISTER COMMA REGISTER COMMA IMM
{
  $$.op = OP_SUB;
  $$.ops[0] = OP_REG($2);  
  $$.ops[1] = OP_REG($4);  
  $$.ops[2] = OP_IMM($6);  
}
	     | TRAP IMM
{
  $$.op = OP_TRAP;
  $$.ops[0] = OP_IMM($2);  
}
	     | ORIG IMM
{
  $$.op = OP_ORIG;
  $$.ops[0] = OP_IMM($2);  
}
             | HALT
{
  $$.op = OP_HALT;
}
             | END
{
  $$.op = OP_END;
}
             | IN
{
  $$.op = OP_IN;
}
             | OUT
{
  $$.op = OP_OUT;
}
             | GETC
{
  $$.op = OP_GETC;
}
             | PUTS
{
  $$.op = OP_PUTS;
}
             | PUTSP
{
  $$.op = OP_PUTSP;
}
;

%%



int yywrap() {
  static FILE * currentFile = NULL;

  if(infile[0]==NULL)
    return 1;
  
  currentFile = fopen(infile[0],"r");

  if(currentFile!=NULL)
    yyin = currentFile;
  else
    printf("Could not open file: %s",infile[0]);

  infile[0] = NULL;
  
  return (currentFile)?0:1;
}


int yyerror()
{
  num_errors++;
  printf("Un-resolved syntax error: %d.\n",line_num);
  return 1;
}


operand build_operand(enum op_type t, void *data)
{
  operand op;
  op.t = t;
  switch(t) {
  case op_reg:
    op.reg = (int) data;
    break;
  case op_imm:  
    op.imm = (int) data;
    break;
  case op_label:
    op.label = (char *) data;
    break;
  }
  return op;
}

inst_t new_inst(inst_t copy)
{
  inst_t tmp = (inst_t) malloc( sizeof(*copy) );
  *tmp = *copy;
  return tmp;
}

inst_t append_inst(inst_t head, inst_t item)
{
  inst_t tmp = head;

  if(head == NULL)
    return item;

  while(tmp && tmp->next != NULL)
    tmp = tmp->next;
  
  tmp->next = item;
  
  return head; 
}

