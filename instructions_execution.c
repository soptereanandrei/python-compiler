#include "instructions_execution.h"

void execute_block(BLOCK_OF_INSTRS *block) {
    INSTR_LIST_NODE *instr_node = block->first;

    while (instr_node) {
        switch (instr_node->instr->type)
        {
            case ASSIGN_TYPE:
            //printf("b1\n");
                execute_assignment(instr_node->instr->instr);
                break;

            case IF_TYPE:
            //printf("b2\n");
                execute_if(instr_node->instr->instr);
                break;

            case WHILE_TYPE:
            //printf("b3\n");
                execute_while(instr_node->instr->instr);
                break;

            case FOR_TYPE:
            //printf("b4\n");
                execute_for(instr_node->instr->instr);
                break;

            case PRINT_TYPE:
            //printf("b5\n");
                execute_print(instr_node->instr->instr);
                break;
            
            default:
                printf("error: Invalid instruction type\n");
                break;
        }

        instr_node = instr_node->next;
    }
}

void execute_assignment(ASSIGN_INSTR *instr) {
    int32_t value = eval_expr(instr->expr);
    *(instr->addr) = value;
}

void execute_if(IF_INSTR *instr) {
    int32_t cond_eval = eval_expr(instr->condition);
    if (cond_eval) {
        execute_block(instr->then_body);
    } else {
        if (instr->else_instr != NULL)
            execute_block(instr->else_instr->body);
    }
}

void execute_while(WHILE_INSTR *instr) {
    while (eval_expr(instr->condition))
        execute_block(instr->body);
}

void execute_for(FOR_INSTR *instr) {
    *(instr->identificator_addr) = eval_expr(instr->iterator->init);

    while (*(instr->identificator_addr) != eval_expr(instr->iterator->end_cond)) {
        execute_block(instr->body);
        int32_t step = eval_expr(instr->iterator->step);
        *(instr->identificator_addr) += step;
    }
}

void execute_print(PRINT_INSTR *instr) {
   if (instr->expr != NULL) {
        printf("%d", eval_expr(instr->expr));
    } else {
        
        if (strcmp(instr->string, "\\n") == 0)
            printf("\n");
        else
            printf("%s", instr->string);
    }
}