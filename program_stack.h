#ifndef PROGRAM_STACK_GUARD
#define PROGRAM_STACK_GUARD

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define STACK_SIZE 1024

typedef struct _PROGRAM_STACK {
    uint32_t *top; //top of the stack
    uint32_t *bottom; //bottom of the stack
    uint32_t ESP;
    uint32_t EBP;
}PROGRAM_STACK;

typedef struct _ID_MAP {
    char                *name;
    uint32_t            *addr;
    struct _ID_MAP      *next;
}ID_MAP;

void init_program_stack();

uint32_t *add_identifier(char *name);
uint32_t *get_identifier_addr(char *name);
uint32_t *resolve_identifier_addr(char *name);

#endif