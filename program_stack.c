#include "program_stack.h"

PROGRAM_STACK *stack;
ID_MAP *first;
ID_MAP *last;

void init_program_stack() {
    stack = (PROGRAM_STACK*)malloc(sizeof(PROGRAM_STACK));

    //alocate the stack
    stack->top = (uint32_t*)malloc(sizeof(uint32_t) * STACK_SIZE);
    stack->bottom = stack->top + 1024 - 1;
    stack->EBP = stack->ESP = 1024 - 1;

    first = NULL;
    last = NULL;
}

void add_map(ID_MAP *map) {
    if (first == NULL && last == NULL) {
        first = last = map;
        return;
    }

    last->next = map;
    last = map;
}

uint32_t *add_identifier(char *name) {
    ID_MAP *map = (ID_MAP*)malloc(sizeof(ID_MAP));

    map->name = name;
    stack->ESP -= 1;
    map->addr = (stack->top + stack->ESP + 1);
    map->next = NULL;

    add_map(map);

    return map->addr;
}

uint32_t *get_identifier_addr(char *name) {
    ID_MAP *map = first;

    while (map) {
        if (strcmp(map->name, name) == 0) {
            return map->addr;
        }

        map = map->next;
    }
    
    return NULL;
}

uint32_t *resolve_identifier_addr(char *name) {
    uint32_t *addr = get_identifier_addr(name);

    if (addr)
        return addr;

    return add_identifier(name);
}