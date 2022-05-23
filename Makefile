NAME = python_compiler
SOURCES = lex.yy.c y.tab.c instructions.c program_stack.c instructions_execution.c
LEX_FILE = lexfile.l
YACC_FILE = yaccfile.y
OBJS = $(SOURCES:.c=.o)

all:
	lex $(LEX_FILE)
	yacc -d $(YACC_FILE)
	gcc -o $(NAME) $(SOURCES) -I ./ -lfl -ly
	
clean:
	rm -rf lex.yy.c y.tab.c y.tab.h

fclean: clean
	rm -rf $(NAME)

re: fclean all