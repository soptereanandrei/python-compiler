#ifndef INSTRUCTIONS_GUARD
#define INSTRUCTIONS_GUARD

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "program_stack.h"

enum _EXPR_TYPE {IMM_VAL, MEM, ARITH_COMPOUND, NOT_NULL, COND_COMPOUND};
typedef enum _EXPR_TYPE EXPR_TYPE;

typedef struct _EXPRESSION {
    EXPR_TYPE type; //expression can be a simple operand or a compound expression
    union {
        int32_t l_operand_imm;
        int32_t *l_operand_addr;
        struct _EXPRESSION *l_operand_expr;
    };
    char *_operator;
    struct _EXPRESSION *r_operand;
}EXPRESSION;


EXPRESSION *create_expr_imm(int32_t operand);
EXPRESSION *create_expr_addr(int32_t *operand);
EXPRESSION *create_expr_expr(EXPRESSION *l_expr, char *op, EXPRESSION *r_expr);
EXPRESSION *create_expr_notnull(EXPRESSION *l_expr);
EXPRESSION *create_expr_expr_cond(EXPRESSION *l_expr, char *op, EXPRESSION *r_expr);

int32_t eval_expr(EXPRESSION *expr);

enum _INSTR_TYPE {ASSIGN_TYPE, IF_TYPE, ELSE_TYPE, FOR_TYPE, WHILE_TYPE, PRINT_TYPE};
typedef enum _INSTR_TYPE INSTR_TYPE;

typedef struct _ABSTRACT_INSTR {
    INSTR_TYPE              type;
    void                    *instr;
}ABSTRACT_INSTR;

typedef struct _INSTR_LIST_NODE { //wrapper of ABSTRACT_INSTR to create list of it
    ABSTRACT_INSTR          *instr;
    struct _INSTR_LIST_NODE *next;
}INSTR_LIST_NODE;

typedef struct _BLOCK_OF_INSTRS {
    size_t                  indentation;
    INSTR_LIST_NODE         *first;
    INSTR_LIST_NODE         *last;
    struct _BLOCK_OF_INSTRS *parent;
}BLOCK_OF_INSTRS;

typedef struct _ASSIGN_INSTR {
    uint32_t    *addr;
    EXPRESSION  *expr;
}ASSIGN_INSTR;

typedef struct _ELSE_INSTR {
    EXPRESSION          *condition; //it may be null
    BLOCK_OF_INSTRS     *body;
}ELSE_INSTR;

typedef struct _IF_INSTR {
    EXPRESSION      *condition;
    BLOCK_OF_INSTRS *then_body;
    ELSE_INSTR      *else_instr;
}IF_INSTR;

typedef struct _WHILE_INSTR {
    EXPRESSION      *condition;
    BLOCK_OF_INSTRS *body;
}WHILE_INSTR;

typedef struct _RANGE_ITERATOR {
    EXPRESSION      *init;
    EXPRESSION      *end_cond;
    EXPRESSION      *step;
}RANGE_ITERATOR;

typedef struct _FOR_INSTR {
    uint32_t        *identificator_addr;
    RANGE_ITERATOR  *iterator;
    BLOCK_OF_INSTRS *body;
}FOR_INSTR;

typedef struct _PRINT_INSTR {
    EXPRESSION *expr;
    const char *string;
}PRINT_INSTR;

typedef struct _ELSE_INSTR_STACK_NODE {
    ELSE_INSTR                      *else_instr;
    struct _ELSE_INSTR_STACK_NODE   *prev;
}ELSE_INSTR_STACK_NODE;

typedef struct _BLOCK_OF_INSTRS_STACK_NODE {
    BLOCK_OF_INSTRS                     *block;
    struct _BLOCK_OF_INSTRS_STACK_NODE  *prev;
}BLOCK_OF_INSTRS_STACK_NODE;

typedef struct _PROGRAM_FLOW_CONTROL {
    BLOCK_OF_INSTRS             *root;
    BLOCK_OF_INSTRS_STACK_NODE  *blocks_stack_head;
    ELSE_INSTR_STACK_NODE       *elses_stack_head;
}PROGRAM_FLOW_CONTROL;

void init();

void push_instr(ABSTRACT_INSTR *instr, int indentation);

ABSTRACT_INSTR *create_assign_instr(char *id, EXPRESSION *expr);
ABSTRACT_INSTR *create_if_instr(EXPRESSION *cond);
ABSTRACT_INSTR *create_else_instr(EXPRESSION *cond);
ABSTRACT_INSTR *create_while_instr(EXPRESSION *cond);
ABSTRACT_INSTR *create_for_instr(uint32_t *addr, RANGE_ITERATOR *iterator);
ABSTRACT_INSTR *create_print_instr(EXPRESSION *expr, const char *string);

RANGE_ITERATOR *create_range_iterator(EXPRESSION *init, EXPRESSION *end_cond, EXPRESSION *step);

void show_block(BLOCK_OF_INSTRS *block);

void debug();
BLOCK_OF_INSTRS *get_root_block();

#endif
