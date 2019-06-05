%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern FILE *yyin;
int yylexeNode(void);
void yyerror(char *s);
%}
%union {
    char* strval;
};

%token <strval>  NUM IDENTIFIER

%token LE_OP GE_OP EQ_OP NE_OP

%token IF ELSE  WHILE  CONTINUE BREAK RETURN VOID INT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%start program
%%

program
    : declaration_list { printf("program declaration_list\n"); }
    ;

declaration_list
    : declaration_list declaration { printf("declaration_list declaration_list declaration\n");; }
    | declaration { printf("declaration_list declaration\n"); }
    ;

declaration
    : var_declaration 
        { 
            printf("declaration var_declaration\n");
        }
    | fun_declaration { printf("declaration fun_declaration\n"); }
    ;

var_declaration
    : type_specifier IDENTIFIER ';'
        {
            printf("var_declaration type_specifier ID:%s ;\n",$2);
        }
    | type_specifier IDENTIFIER '[' NUM ']' ';'
        {
            printf("var_declaration type_specifier ID:%s '[' NUM:%s ']' ';'\n",$2, $4);
        }
    ;

type_specifier
    : INT { printf("type_specifier INT\n"); }
    | VOID { printf("type_specifier VOID\n"); }
    ;

fun_declaration
    : type_specifier IDENTIFIER '(' params ')' compound_stmt
        {
            printf("fun_declaration type_specifier ID:%s '(' params ')' compound_stmt\n", $2); 
        }
    ;

params
    : params_list { printf("params params_list\n"); }
    | VOID { printf("params VOID\n"); }
    ;

params_list
    : params_list ',' param { printf("params_list params_list ',' param\n"); }
    | param { printf("params_list param\n"); }
    ;

param
    : type_specifier IDENTIFIER
        {
            printf("param type_specifier ID:%s\n",$2);
        }
    | type_specifier IDENTIFIER '[' ']'
        {
            printf("param type_specifier ID:%s '[' ']'\n",$2);
        }
    ;

compound_stmt
    : '{' local_declarations statement_list '}' 
        {printf("compound-stmt           { local-declarations statement-list }\n"); }
    ;

local_declarations
    : local_declarations var_declaration {printf("local-declarations      local-declarations var-declaration\n"); }
    | /* empty */ {printf("local-declarations      empty\n"); }
    ;

statement_list
    : statement_list statement {printf("statement-list          statement-list statement\n"); }
    | /* empty */ {printf("statement               expression-stmt\n"); }
    ;

statement
    : expression_stmt {printf("statement               expression-stmt\n"); }
    | compound_stmt {printf("statement               compound-stmt\n"); }
    | selection_stmt {printf("statement               selection-stmt\n"); }
    | iteration_stmt {printf("statement               iteration-stmt\n"); }
    | return_stmt {printf("statement               return-stmt\n"); }
    ;

expression_stmt
    : expression ';' {printf("expression-stmt         expression ;\n"); }
    | ';' {printf("expression-stmt         ;\n"); }
    ;

selection_stmt
    : IF '(' expression ')' statement {printf("selection-stmt          if ( expression ) statement\n"); }
    | IF '(' expression ')' statement ELSE statement
        {printf("selection-stmt          if ( expression ) statement else statement\n") }
    ;

iteration_stmt
    : WHILE '(' expression ')' statement {
        printf("iteration-stmt          while ( expression ) statement\n"); }
    ;

return_stmt
    : RETURN ';' {printf("return-stmt             return ;\n"); }
    | RETURN expression ';' {printf("return-stmt             return expression ;\n"); }
    ;

expression
    : var '=' expression
        {
           printf("expression              var = expression\n");
        }
    | simple_expression {printf("expression              simple-expression\n"); }
    ;

var
    : IDENTIFIER {printf("var                     ID:%s\n",$1); }
    | IDENTIFIER '[' expression ']' 
        {
           printf("var                     ID:%s [ expression ]\n",$1);
        }
    ;

simple_expression
    : additive_expression relop additive_expression
        {
           printf("simple-expression  additive-expression relop additive-expression\n")
        }
    | additive_expression {printf("simple-expression  additive-expression\n"); }
    ;

relop
    : LE_OP {printf("relop <=\n"); }
    | '<' {printf("relop <\n"); }
    | '>' {printf("relop >\n"); }
    | GE_OP {printf("relop >=\n"); }
    | EQ_OP {printf("relop ==\n"); }
    | NE_OP {printf("relop !=\n"); }
    ;

additive_expression
    : additive_expression addop term
        {
           printf("additive-expression  additive-expression addop term\n");
        }
    | term
        { 
           printf("additive-expression term\n");
        }
    ;

addop
    : '+' {printf("addop  +\n"); }
    | '-' {printf("addop -\n"); }
    ;

term
    : term mulop factor
        {
           printf("term  term mulop factor\n");
        }
    | factor {printf("term factor\n"); }
    ;

mulop
    : '*' {printf("mulop *\n"); }
    | '/' {printf("mulop /\n"); }
    ;

factor
    : '(' expression ')' {printf("factor  (expression)\n"); }
    | var {printf("factor var\n"); }
    | call {printf("factor call\n"); }
    | NUM {printf("factor NUM:%s\n", $1); }
    ;

call
    : IDENTIFIER '(' args ')'
        { 
           printf("call  ID:%s ( args )\n",$1);
        }
    ;

args
    : arg_list {printf("args  arg-list\n"); }
    | /* empty */ {printf("args empty\n"); }
    ;

arg_list
    : arg_list ',' expression {printf("arg-list  arg-list, expression\n"); }
    | expression {printf("arg-list expression\n"); }
    ;

%%

void yyerror(char *s) 
{
   printf("%s\n", s);
}

int main(int argc,char * argv[]) 
{

    yyin = fopen(argv[1],"r");
    // hash_init(var_local, HASHSIZE);
    // hash_init(var_local_SorA, HASHSIZE);
    // hash_init(var_local_GorP, HASHSIZE);
    yyparse();
    fclose(yyin);
    return 0;
}
