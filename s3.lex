%{
#include "s3inst.h"
#include "s3.y.h"
#include <stdio.h>
#include <string.h>
int line_num=0;
 char *trash;
%}


%x garbage ops

CCODE    [Nn]?[Zz]?[Pp]?

/* operand types */
REGISTER [rR][0-9]+
HEX      [xX][-]?[0-9a-fA-F]+
DECIMAL  [#]?[-]?[0-9]+
IMMED    {HEX}|{DECIMAL}
LABEL    [A-Za-z_][A-Za-z_0-9]*

/* strings with no carriage returns */
STRING   \"([^"\n\r\\]*\\[^\n\r])*[^"\n\r\\]*\" 

/* operand and white space specification */
SPACE    [ \t]
COMMENT  [;][^\n\r]*
ENDLINE  {COMMENT}?\r?\n\r?


%%

{SPACE}+            ;
<ops>{SPACE}+       ;
 
:                   ;    
<ops>=              { return EQ; }
<ops>,              { return COMMA; }
\(                  { return LPAREN; }
\)                  { return RPAREN; }
{ENDLINE}+          { line_num++; BEGIN(0); return ENDLINE; }
<ops>{ENDLINE}+          { line_num++; BEGIN(0); return ENDLINE; }
<ops>\.                  { BEGIN(0); return INST_SEP; }

<ops>[nN]                { yylval.nzp = CC_N; return BR_CC; }
<ops>[zZ]                { yylval.nzp = CC_Z; return BR_CC; }
<ops>[pP]                { yylval.nzp = CC_P; return BR_CC; }
<ops>[nN][pP]            { yylval.nzp = CC_N | CC_P; return BR_CC; }
<ops>[zZ][pP]            { yylval.nzp = CC_Z | CC_P; return BR_CC; }
<ops>[nN][zZ]            { yylval.nzp = CC_Z | CC_N; return BR_CC; }
<ops>[nN][zZ][pP]        { yylval.nzp = CC_N | CC_Z | CC_P; return BR_CC; }

[aA][dD][dD]        { BEGIN(ops);  return ADD;  }
[aA][nN][dD]        { BEGIN(ops); return AND;  }
[aA][nN][dD][lL]    { BEGIN(ops); return ANDL; }
[bB][rR]            { BEGIN(ops); return BR;   }
[bB][rR][aA]        { BEGIN(ops); return BRA;  }
[dD][iI][vV]        { BEGIN(ops); return DIV; }
[jJ][mM][pP]        { BEGIN(ops); return JMP; }
[jJ][sS][rR][rR]    { BEGIN(ops); return JSRR; }
[jJ][sS][rR]        { BEGIN(ops); return JSR; }
[lL][dD][iI]        { BEGIN(ops); return LDI; }
[lL][dD][rR]        { BEGIN(ops); return LDR; }
[lL][dD]            { BEGIN(ops); return LD; }
[lL][eE][aA]        { BEGIN(ops); return LEA; }
[mM][uU][lL]        { BEGIN(ops); return MUL; }
[nN][oO][pP]        { BEGIN(ops); return NOP; }
[nN][oO][tT]        { BEGIN(ops); return NOT; }
[nN][oO][tT][lL]    { BEGIN(ops); return NOTL; }
[oO][rR]            { BEGIN(ops); return OR; }
[oO][rR][lL]        { BEGIN(ops); return ORL; }
[rR][tT][iI]        { BEGIN(ops); return RTI; }
[rR][eE][tT]        { BEGIN(ops); return RET; }
[sS][eE][tT]        { BEGIN(ops); return SET; }
[sS][tT][iI]        { BEGIN(ops); return STI; }
[sS][tT][rR]        { BEGIN(ops); return STR; }
[sS][tT]            { BEGIN(ops); return ST; }
[sS][uU][bB]        { BEGIN(ops); return SUB; }
[tT][rR][aA][pP]    { BEGIN(ops); return TRAP; }

[hH][aA][lL][tT]    { BEGIN(ops); return HALT; }
[iI][nN]            { BEGIN(ops); return IN; }
[oO][uU][tT]        { BEGIN(ops); return OUT; }

[gT][eE][tT][cC]     { BEGIN(ops); return GETC; }
[pP][uU][tT][sS]     { BEGIN(ops); return PUTS; }
[pP][uU][tT][sS][pP] { BEGIN(ops); return PUTSP; }

\.[oO][rR][iI][gG]  { BEGIN(ops); return ORIG; }
\.[eE][nN][dD]      { return END; }

<ops>#[-]?[0-9]+       { yylval.imm = atoi(yytext+1); return IMM; }
<ops>[-]?[0-9]+        { yylval.imm = atoi(yytext);   return IMM; }
<ops>{HEX}             { yylval.imm = strtol(yytext+1,&trash,16); return IMM; }
<ops>{REGISTER}        { yylval.reg = atoi(yytext+1); return REGISTER; }
<ops>{LABEL}           { yylval.label = strdup (yytext); return LABEL;}
<ops>{STRING}          { yylval.string = strdup(yytext); return STRING; }

{LABEL}:               { yylval.label = strdup (yytext); 
                        *(strchr(yylval.label,':')) = '\0'; 
                        return LABEL;}

{LABEL}{SPACE}+:      { yylval.label = strdup (yytext); 
                        *(strchr(yylval.label,' ')) = '\0'; 
                        return LABEL;}

[-]?[0-9]+        { yylval.imm = atoi(yytext);   return IMM; }
%%
