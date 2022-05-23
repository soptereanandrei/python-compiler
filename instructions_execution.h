#ifndef INSTRUCTIONS_EXECUTION_GUARD
#define INSTRUCTIONS_EXECUTION_GUARD

#include "instructions.h"

void execute_block(BLOCK_OF_INSTRS *block);
void execute_assignment(ASSIGN_INSTR *instr);
void execute_if(IF_INSTR *instr);
void execute_while(WHILE_INSTR *instr);
void execute_for(FOR_INSTR *instr);
void execute_print(PRINT_INSTR *instr);

#endif