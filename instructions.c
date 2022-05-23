#include "instructions.h"

PROGRAM_FLOW_CONTROL program_flow_control;

EXPRESSION *create_expr_imm(int32_t operand) {
    EXPRESSION *expr = (EXPRESSION*)malloc(sizeof(EXPRESSION));
    if (expr == NULL) {
        printf("malloc error, exit\n");
        exit(1);
    }

    expr->type = IMM_VAL;
    expr->l_operand_imm = operand;
    expr->_operator = NULL;
    expr->r_operand = NULL;

    return expr;
}

EXPRESSION *create_expr_addr(int32_t *operand) {
    EXPRESSION *expr = (EXPRESSION*)malloc(sizeof(EXPRESSION));
    if (expr == NULL) {
        printf("malloc error, exit\n");
        exit(1);
    }

    expr->type = MEM;
    expr->l_operand_addr = operand;
    expr->_operator = NULL;
    expr->r_operand = NULL;

    return expr;
}

EXPRESSION *create_expr_expr(EXPRESSION *l_expr, char *op, EXPRESSION *r_expr) {
    EXPRESSION *expr = (EXPRESSION*)malloc(sizeof(EXPRESSION));
    if (expr == NULL) {
        printf("malloc error, exit\n");
        exit(1);
    }

    expr->type = ARITH_COMPOUND;
    expr->l_operand_expr = l_expr;
    expr->_operator = op;
    expr->r_operand = r_expr;

    return expr;
}

EXPRESSION *create_expr_notnull(EXPRESSION *l_expr) {
    EXPRESSION *expr = (EXPRESSION*)malloc(sizeof(EXPRESSION));
    if (expr == NULL) {
        printf("malloc error, exit\n");
        exit(1);
    }

    expr->type = NOT_NULL;
    expr->l_operand_expr = l_expr;
    expr->_operator = NULL;
    expr->r_operand = NULL;

    return expr;
}

EXPRESSION *create_expr_expr_cond(EXPRESSION *l_expr, char *op, EXPRESSION *r_expr) {
    EXPRESSION *expr = (EXPRESSION*)malloc(sizeof(EXPRESSION));
    if (expr == NULL) {
        printf("malloc error, exit\n");
        exit(1);
    }

    expr->type = COND_COMPOUND;
    expr->l_operand_expr = l_expr;
    expr->_operator = op;
    expr->r_operand = r_expr;

    return expr;
}

int32_t eval_expr(EXPRESSION *expr) {
    if (expr->type == IMM_VAL)
        return expr->l_operand_imm;

    if (expr->type == MEM)
        return *(expr->l_operand_addr);

    if (expr->type == ARITH_COMPOUND) {
        int32_t lvalue = eval_expr(expr->l_operand_expr);
        int32_t rvalue = eval_expr(expr->r_operand);

        if (strcmp(expr->_operator, "+") == 0) {
            return lvalue + rvalue;
        } else if (strcmp(expr->_operator, "-") == 0) {
            return lvalue - rvalue;
        } else if (strcmp(expr->_operator, "*") == 0) {
            return lvalue * rvalue;
        } else if (strcmp(expr->_operator, "/") == 0) {
            return lvalue / rvalue;
        } else if (strcmp(expr->_operator, "%") == 0) {
            return lvalue % rvalue;
        }
    }

    if (expr->type == NOT_NULL) {
        int32_t value = eval_expr(expr->l_operand_expr);

        return value != 0;
    }

    if (expr->type == COND_COMPOUND) {
        int32_t lvalue = eval_expr(expr->l_operand_expr);
        int32_t rvalue = eval_expr(expr->r_operand);

        if (strcmp(expr->_operator, "or") == 0) {
            return lvalue || rvalue;
        } else if (strcmp(expr->_operator, "and") == 0) {
            return lvalue && rvalue;
        } else if (strcmp(expr->_operator, "<") == 0) {
            return lvalue < rvalue;
        } else if (strcmp(expr->_operator, "<=") == 0) {
            return lvalue <= rvalue;
        } else if (strcmp(expr->_operator, ">") == 0) {
            return lvalue > rvalue;
        } else if (strcmp(expr->_operator, ">=") == 0) {
            return lvalue >= rvalue;
        } else if (strcmp(expr->_operator, "!=") == 0) {
            return lvalue != rvalue;
        } else if (strcmp(expr->_operator, "==") == 0) {
            return lvalue == rvalue;
        }
    }

    return 0;
}

BLOCK_OF_INSTRS *create_new_block();

void init() {
    program_flow_control.root = create_new_block();
    program_flow_control.root->indentation = 0;
    program_flow_control.blocks_stack_head = NULL;
    program_flow_control.elses_stack_head = NULL;

    BLOCK_OF_INSTRS_STACK_NODE *node = (BLOCK_OF_INSTRS_STACK_NODE*)malloc(sizeof(BLOCK_OF_INSTRS_STACK_NODE));
    node->block = program_flow_control.root;
    node->prev = NULL;

    program_flow_control.blocks_stack_head = node;
}

BLOCK_OF_INSTRS *create_new_block() {
    BLOCK_OF_INSTRS *block = (BLOCK_OF_INSTRS*)malloc(sizeof(BLOCK_OF_INSTRS));

    block->indentation = 0;
    block->parent = NULL;
    block->first = NULL;
    block->last = NULL;

    return block;
}

void push_front_instr_to_block(BLOCK_OF_INSTRS *block, ABSTRACT_INSTR *instr) {
    INSTR_LIST_NODE *instr_node = (INSTR_LIST_NODE*)malloc(sizeof(INSTR_LIST_NODE));

    instr_node->instr = instr;
    instr_node->next = NULL;
    
    if (block->first == NULL && block->last == NULL) {
        block->first = block->last = instr_node;
        return;
    }

    instr_node->next = block->first;
    block->first = instr_node;
}

BLOCK_OF_INSTRS *head_block(PROGRAM_FLOW_CONTROL *program_flow_control) {
    BLOCK_OF_INSTRS_STACK_NODE *node = program_flow_control->blocks_stack_head;

    if (node == NULL)
        return NULL;
    
    return node->block;
}

void pop_block(PROGRAM_FLOW_CONTROL *program_flow_control) {
    BLOCK_OF_INSTRS_STACK_NODE *node = program_flow_control->blocks_stack_head;

    if (node == NULL)
        return;

    program_flow_control->blocks_stack_head = program_flow_control->blocks_stack_head->prev;

    free(node);
}

void push_block(PROGRAM_FLOW_CONTROL *program_flow_control, int indentation) {
    BLOCK_OF_INSTRS_STACK_NODE *node = (BLOCK_OF_INSTRS_STACK_NODE*)malloc(sizeof(BLOCK_OF_INSTRS_STACK_NODE));
    
    BLOCK_OF_INSTRS *block = create_new_block();
    block->indentation = indentation;

    node->block = block;
    node->prev = program_flow_control->blocks_stack_head;
    program_flow_control->blocks_stack_head = node;
}

ELSE_INSTR *head_else(PROGRAM_FLOW_CONTROL *program_flow_control) {
    ELSE_INSTR_STACK_NODE *node = program_flow_control->elses_stack_head;

    if (node == NULL)
        return NULL;

    return node->else_instr;
}

void pop_else(PROGRAM_FLOW_CONTROL *program_flow_control) {
    ELSE_INSTR_STACK_NODE *node = program_flow_control->elses_stack_head;

    if (node == NULL)
        return;

    program_flow_control->elses_stack_head = program_flow_control->elses_stack_head->prev;

    free(node);
}

void push_else(PROGRAM_FLOW_CONTROL *program_flow_control, ELSE_INSTR *else_instr) {
    ELSE_INSTR_STACK_NODE *node = (ELSE_INSTR_STACK_NODE*)malloc(sizeof(ELSE_INSTR_STACK_NODE));

    node->else_instr = else_instr;
    node->prev = program_flow_control->elses_stack_head;
    program_flow_control->elses_stack_head = node;
}

void push_instr(ABSTRACT_INSTR *instr, int indentation) {
    //printf("1 indentation=%d\n", indentation);
    BLOCK_OF_INSTRS *head = head_block(&program_flow_control);

    if (head->indentation == indentation) {
        //printf("2\n");
        push_front_instr_to_block(head, instr);
    } else if (indentation > head->indentation) {
        //printf("3\n");
        //printf("oldhead=%d\n", head);
        for (int i = head->indentation + 1; i <= indentation; i++)
            push_block(&program_flow_control, i);
        //printf("4\n");
        BLOCK_OF_INSTRS *newhead = head_block(&program_flow_control);
        //printf("newhead=%d\n", newhead);
        push_front_instr_to_block(newhead, instr);
    } else if (indentation == head->indentation - 1) {
        //printf("5\n");
        pop_block(&program_flow_control);
        switch (instr->type)
        {
            case IF_TYPE:
                IF_INSTR* if_instr = (IF_INSTR*)instr->instr;
                if_instr->then_body = head;
                
                ELSE_INSTR *top_else = head_else(&program_flow_control);
                if (top_else != NULL && top_else->body->indentation == indentation + 1) {
                    if_instr->else_instr = top_else;
                    pop_else(&program_flow_control);
                }

                push_front_instr_to_block(head_block(&program_flow_control), instr);
                break;
            
            case ELSE_TYPE:
                ELSE_INSTR *else_instr = (ELSE_INSTR*)instr->instr;
                else_instr->body = head;
                push_else(&program_flow_control, else_instr);
                break;

            case WHILE_TYPE:
                WHILE_INSTR *while_instr = (WHILE_INSTR*)instr->instr;
                while_instr->body = head;
                push_front_instr_to_block(head_block(&program_flow_control), instr);
                break;

            case FOR_TYPE:
                FOR_INSTR *for_instr = (FOR_INSTR*)instr->instr;
                for_instr->body = head;
                push_front_instr_to_block(head_block(&program_flow_control), instr);
                break;

            default:
                printf("invalid instruction\n");
                break;
        }
    } else {
        printf("indentation error, current identation is %d but it should to be %d\n", indentation, head->indentation - 1);
    }
    //printf("end\n");
}

ABSTRACT_INSTR *create_assign_instr(char *var_name, EXPRESSION *expr) {
    ASSIGN_INSTR *assign_instr = (ASSIGN_INSTR*)malloc(sizeof(ASSIGN_INSTR));

    uint32_t *addr = get_identifier_addr(var_name);
    if (addr == NULL)
        addr = add_identifier(var_name);

    assign_instr->addr = addr;
    assign_instr->expr = expr;
    
    ABSTRACT_INSTR *instr = (ABSTRACT_INSTR*)malloc(sizeof(ABSTRACT_INSTR));

    instr->type = ASSIGN_TYPE;
    instr->instr = assign_instr;

    return instr;
}

ABSTRACT_INSTR *create_if_instr(EXPRESSION *cond) {
    IF_INSTR *if_instr = (IF_INSTR*)malloc(sizeof(IF_INSTR));

    if_instr->condition = cond;
    if_instr->then_body = NULL;
    if_instr->else_instr = NULL;
    
    ABSTRACT_INSTR *instr = (ABSTRACT_INSTR*)malloc(sizeof(ABSTRACT_INSTR));

    instr->type = IF_TYPE;
    instr->instr = if_instr;

    return instr;
}

ABSTRACT_INSTR *create_else_instr(EXPRESSION *cond) {
    ELSE_INSTR *else_instr = (ELSE_INSTR*)malloc(sizeof(ELSE_INSTR));

    else_instr->condition = cond;
    else_instr->body = NULL;

    ABSTRACT_INSTR *instr = (ABSTRACT_INSTR*)malloc(sizeof(ABSTRACT_INSTR));

    instr->type = ELSE_TYPE;
    instr->instr = else_instr;

    return instr;
}

ABSTRACT_INSTR *create_while_instr(EXPRESSION *cond) {
    WHILE_INSTR *while_instr = (WHILE_INSTR*)malloc(sizeof(WHILE_INSTR));

    while_instr->condition = cond;
    while_instr->body = NULL;

    ABSTRACT_INSTR *instr = (ABSTRACT_INSTR*)malloc(sizeof(ABSTRACT_INSTR));

    instr->type = WHILE_TYPE;
    instr->instr = while_instr;

    return instr;
}

ABSTRACT_INSTR *create_for_instr(uint32_t *addr, RANGE_ITERATOR *iterator) {
    FOR_INSTR *for_instr = (FOR_INSTR*)malloc(sizeof(FOR_INSTR));

    for_instr->identificator_addr = addr;
    for_instr->iterator = iterator;
    for_instr->body = NULL;

    ABSTRACT_INSTR *instr = (ABSTRACT_INSTR*)malloc(sizeof(ABSTRACT_INSTR));

    instr->type = FOR_TYPE;
    instr->instr = for_instr;

    return instr;
}

RANGE_ITERATOR *create_range_iterator(EXPRESSION *init, EXPRESSION *end_cond, EXPRESSION *step) {
    RANGE_ITERATOR *iterator = (RANGE_ITERATOR*)malloc(sizeof(RANGE_ITERATOR));

    init = init != NULL ? init : create_expr_imm(0);
    step = step != NULL ? step : create_expr_imm(1);

    iterator->init = init;
    iterator->end_cond = end_cond;
    iterator->step = step;

    return iterator;
}

ABSTRACT_INSTR *create_print_instr(EXPRESSION *expr, const char *string) {
    PRINT_INSTR *print_instr = (PRINT_INSTR*)malloc(sizeof(PRINT_INSTR));

    if (expr != NULL) {
        print_instr->expr = expr;
        print_instr->string = NULL;
    } else {
        print_instr->expr = NULL;
        print_instr->string = string;
    }

    ABSTRACT_INSTR *instr = (ABSTRACT_INSTR*)malloc(sizeof(ABSTRACT_INSTR));

    instr->type = PRINT_TYPE;
    instr->instr = print_instr;

    return instr;
}

void show_block(BLOCK_OF_INSTRS *block) {
    INSTR_LIST_NODE *node = block->first;

    while (node) {
        for (int i = 0; i < block->indentation; i++)
            printf("\t");

        if (node->instr->type == ASSIGN_TYPE) {
            printf("ASSIGNATION\n");
        } else if (node->instr->type == IF_TYPE) {
            printf("IF\n");
            IF_INSTR *if_instr = (IF_INSTR*)node->instr->instr;
            show_block(if_instr->then_body);
            
            if (if_instr->else_instr != NULL) {
                for (int i = 0; i < block->indentation; i++)
                    printf("\t");
                printf("ELSE\n");
                show_block(if_instr->else_instr->body);
            }
        } else if (node->instr->type == WHILE_TYPE) {
            printf("WHILE\n");
            WHILE_INSTR *while_instr = (WHILE_INSTR*)node->instr->instr;
            show_block(while_instr->body);
        } else if (node->instr->type == FOR_TYPE) {
            printf("FOR\n");
            FOR_INSTR *for_instr = (FOR_INSTR*)node->instr->instr;
            show_block(for_instr->body);
        } else if (node->instr->type == PRINT_TYPE) {
            printf("PRINT\n");
        }

        node = node->next;
    }
}

void debug() {
    show_block(program_flow_control.root);
}

BLOCK_OF_INSTRS *get_root_block() {
    return program_flow_control.root;
}