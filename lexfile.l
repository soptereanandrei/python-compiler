%{
    #include "instructions.h"
    #include "program_stack.h"
    #include "y.tab.h"
%}

%%

[\t]                return TAB;
[\n]                return NEWLINE;

[0]                 { yylval.value = atoi(yytext); return IMM; }
[-]?[1-9]+[0-9]*    { yylval.value = atoi(yytext); return IMM; }
[+-]                { yylval.operator = strdup(yytext); return ARITH_OP_P1; }
[*%/]               { yylval.operator = strdup(yytext); return ARITH_OP_P0; }

[\(]                return OPEN_PARANTHESIS;
[\)]                return CLOSE_PARANTHESIS;

"="                 return ASSIGN;

"or"                { yylval.operator = strdup(yytext); return OR; }
"and"               { yylval.operator = strdup(yytext); return AND; }
"<"|"<="|">"|">="   { yylval.operator = strdup(yytext); return COMP_OP; }
"!="|"=="           { yylval.operator = strdup(yytext); return EQUALITY_OP; }

":"                 return *yytext;
"if"                return IF;
"else"              return ELSE;
"elif"              return ELIF;
"while"             return WHILE;
"for"               return FOR;
"print"             return PRINT;
"range"             return RANGE;
","                 return *yytext;
"in"                return IN;

[\"].*[\"]          { yylval.string = strdup(yytext+1); yylval.string[strlen(yylval.string)-1] = 0; return STRING; }

[a-zA-Z_]+[a-zA-Z0-9_]* { yylval.name = strdup(yytext); return ID; }

.   ;

%%