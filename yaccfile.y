%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "program_stack.h"
  #include "instructions.h"
  #include "instructions_execution.h"
  
  extern int yylex();
  void yyerror(char *s);

%}

%union {
  int                 value;
  char*               operator;
  char*               name;
  char*               string;
  struct _EXPRESSION  *expr;
  ABSTRACT_INSTR      *instr;
  BLOCK_OF_INSTRS     *block_of_instrs;
  RANGE_ITERATOR      *range_iterator;
}

%token NEWLINE TAB ASSIGN PRINT STRING
%token <value> IMM
%token <name> ID

%token OPEN_PARANTHESIS CLOSE_PARANTHESIS
%left OPEN_PARANTHESIS
%left CLOSE_PARANTHESIS

%token <operator> ARITH_OP_P0 ARITH_OP_P1
%left ARITH_OP_P1
%left ARITH_OP_P0

%token <operator> OR AND COMP_OP EQUALITY_OP
%left OR
%left AND
%left COMP_OP
%left EQUALITY_OP

%token IF ELSE ELIF WHILE FOR RANGE IN

%type <value> indentation
%type <instr> instruction assignment if while for print
%type <expr> value arith_expression cond_expression
%type <range_iterator> range_iterator

%%

program : instructions_block
        ;

instructions_block  : indentation instruction end_of_line instructions_block { push_instr($2, $1); }
                    | indentation instruction end_of_line { push_instr($2, $1); }
                    ;

indentation   : indentation TAB { $$ = $1 + 1; }
              | { $$ = 0; }
              ;

instruction : assignment
            | print
            | if
            | while
            | for
            ;

assignment  : ID ASSIGN arith_expression { $$ = create_assign_instr($1, $3); }
            ;

if  : IF cond_expression ':' { $$ = create_if_instr($2); }
    | ELSE ':' { $$ = create_else_instr(NULL); }
    ;

while : WHILE cond_expression ':' { $$ = create_while_instr($2); }
      ;

for   : FOR ID IN range_iterator ':' { $$ = create_for_instr(resolve_identifier_addr($2), $4); }
      ;

print : PRINT OPEN_PARANTHESIS arith_expression CLOSE_PARANTHESIS { $$ = create_print_instr($3, NULL); }
      | PRINT OPEN_PARANTHESIS STRING CLOSE_PARANTHESIS { $$ = create_print_instr(NULL, yylval.string); }
      ;

value : ID { $$ = create_expr_addr(get_identifier_addr($1)); }
      | IMM { $$ = create_expr_imm($1); }
      ;

arith_expression  : OPEN_PARANTHESIS arith_expression CLOSE_PARANTHESIS { $$ = $2; }
                  | arith_expression ARITH_OP_P0 arith_expression { $$ = create_expr_expr($1, $2, $3); }
                  | arith_expression ARITH_OP_P1 arith_expression { $$ = create_expr_expr($1, $2, $3); }
                  | ID { $$ = create_expr_addr(get_identifier_addr($1)); }
                  | IMM { $$ = create_expr_imm($1); }
                  ;

cond_expression   : OPEN_PARANTHESIS cond_expression CLOSE_PARANTHESIS { $$ = $2; }
                  | cond_expression OR cond_expression { $$ = create_expr_expr_cond($1, $2, $3); }
                  | cond_expression AND cond_expression { $$ = create_expr_expr_cond($1, $2, $3); }
                  | arith_expression COMP_OP arith_expression { $$ = create_expr_expr_cond($1, $2, $3); }
                  | arith_expression EQUALITY_OP arith_expression { $$ = create_expr_expr_cond($1, $2, $3); }
                  | arith_expression { $$ = create_expr_notnull($1); }
                  ;

range_iterator  : RANGE OPEN_PARANTHESIS value CLOSE_PARANTHESIS { $$ = create_range_iterator(NULL, $3, NULL); }
                | RANGE OPEN_PARANTHESIS value ',' value CLOSE_PARANTHESIS { $$ = create_range_iterator($3, $5, NULL); }
                | RANGE OPEN_PARANTHESIS value ',' value ',' value CLOSE_PARANTHESIS { $$ = create_range_iterator($3, $5, $7); }
                ;

end_of_line : NEWLINE

%%

int main() {
  //printf("test\n");
  init_program_stack();
  //printf("test\n");
  init();
  //printf("test\n");
  int ret = yyparse();
  //printf("test\n");
  debug();
  execute_block(get_root_block());
  return ret;
}

void yyerror(char *s) {
  printf("%s\n", s);
}

