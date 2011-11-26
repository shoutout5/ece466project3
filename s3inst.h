#ifndef S3INST_H
#define S3INST_H

typedef enum opcode_t opcode_t;
enum opcode_t {
    /* no opcode seen (yet) */
    OP_NONE,

    /* real instruction opcodes */
    OP_ADD, OP_AND, OP_ANDL, OP_BR, OP_BRA,OP_DIV, OP_JMP, OP_JSR, OP_JSRR, OP_LD, OP_LDI, OP_LDR,
    OP_LEA, OP_MUL, OP_NOP, OP_NOT, OP_NOTL, OP_OR, OP_ORL, OP_RTI, OP_SET, OP_ST, OP_STI, OP_STR, OP_SUB, OP_TRAP,

    /* trap pseudo-ops */
    OP_GETC, OP_HALT, OP_IN, OP_OUT, OP_PUTS, OP_PUTSP, OP_END, OP_ORIG,

    /* non-trap pseudo-ops */
    OP_RET, 

    NUM_OPS
};

static const char* const opnames[NUM_OPS] = {
    /* no opcode seen (yet) */
    "missing opcode",
  
    /* real instruction opcodes */
    "ADD", "AND", "ANDL", "BR", "BRA", "DIV", "JMP", "JSR", "JSRR", "LD", "LDI", "LDR", "LEA",
    "MUL", "NOP","NOT", "NOTL", "OR", "ORL", "RTI", "SET", "ST", "STI", "STR", "SUB","TRAP",

    /* trap pseudo-ops */
    "GETC", "HALT", "IN", "OUT", "PUTS", "PUTSP", ".END", ".ORIG",

    /* non-trap pseudo-ops */
    "RET",
};

typedef enum ccode_t ccode_t;
enum ccode_t {
    CC_    = 0,
    CC_P   = 0x0200,
    CC_Z   = 0x0400,
    CC_N   = 0x0800
};

enum op_type {
  op_reg,
  op_imm,
  op_label,
};

typedef struct operand operand;
struct operand {
  enum op_type t;
  union {
    int  reg;
    int  imm;
    char *label;
    int  pred;
  };
};

operand build_operand(enum op_type t, void *data);

#define OP_REG(a) build_operand(op_reg,(void*)a)
#define OP_IMM(a) build_operand(op_imm,(void*)a)
#define OP_LABEL(a) build_operand(op_label,(void*)a)


typedef struct inst_d * inst_t;
struct inst_d {
  char *label;
  opcode_t op;
  int  ccode;
  operand ops[3];
  int data;
  int endOp;
  inst_t next;
};

#endif
