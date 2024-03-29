%token IDENTIFIER CONSTANT STRING_LITERAL
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN

%token TYPEDEF CHAR SHORT INT
%token LONG SIGNED UNSIGNED FLOAT DOUBLE VOID STRUCT ENUM

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR 
%token BREAK RETURN

%start translation_unit
%%

primary_expression : IDENTIFIER
| CONSTANT
| STRING_LITERAL
| '(' expression ')'
;

postfix_expression : primary_expression
| postfix_expression '[' expression ']'
| postfix_expression '(' ')'
| postfix_expression '(' argument_expression_list ')'
| postfix_expression '.' IDENTIFIER
| postfix_expression INC_OP
| postfix_expression DEC_OP
;

argument_expression_list : assignment_expression
| argument_expression_list ',' assignment_expression
;

unary_expression : postfix_expression
| INC_OP unary_expression
| DEC_OP unary_expression
| unary_operator cast_expression
;

unary_operator : '*'
| '+'
| '-'
| '!'
;

cast_expression : unary_expression
| '(' type_name ')' cast_expression
;

multiplicative_expression : cast_expression
| multiplicative_expression '*' cast_expression
| multiplicative_expression '/' cast_expression
| multiplicative_expression '%' cast_expression 
;

additive_expression : multiplicative_expression
| additive_expression '+' multiplicative_expression
| additive_expression '-' multiplicative_expression
;

relational_expression : additive_expression
| relational_expression '<' additive_expression
| relational_expression '>' additive_expression
| relational_expression LE_OP additive_expression
| relational_expression GE_OP additive_expression
;

equality_expression : relational_expression
| equality_expression EQ_OP relational_expression
| equality_expression NE_OP relational_expression
;

logical_and_expression : equality_expression
| logical_and_expression AND_OP equality_expression
;

logical_or_expression : logical_and_expression
| logical_or_expression OR_OP logical_and_expression
;

conditional_expression : logical_or_expression
| logical_or_expression '?' expression ':' conditional_expression
;

assignment_expression : conditional_expression
| unary_expression assignment_operator assignment_expression
;

assignment_operator : '='
| MUL_ASSIGN
| DIV_ASSIGN
| MOD_ASSIGN
| ADD_ASSIGN
| SUB_ASSIGN
;

expression : assignment_expression
| expression ',' assignment_expression
;

declaration : declaration_specifiers ';'
| declaration_specifiers init_declarator_list ';'
;

declaration_specifiers : TYPEDEF
| TYPEDEF declaration_specifiers
| type_specifier
| type_specifier declaration_specifiers
;

init_declarator_list : init_declarator
| init_declarator_list ',' init_declarator
;

init_declarator : direct_declarator
| direct_declarator '=' initializer
;

type_specifier : VOID
| CHAR
| SHORT
| INT
| LONG
| FLOAT
| DOUBLE
| SIGNED
| UNSIGNED
| struct_or_union_specifier
| enum_specifier
;

struct_or_union_specifier : STRUCT IDENTIFIER '{' struct_declaration_list '}'
| STRUCT '{' struct_declaration_list '}'
| STRUCT IDENTIFIER
;

struct_declaration_list : specifier_qualifier_list struct_declarator_list ';'
| struct_declaration_list specifier_qualifier_list struct_declarator_list ';'
;

specifier_qualifier_list : type_specifier specifier_qualifier_list
| type_specifier
;

struct_declarator_list : struct_declarator
| struct_declarator_list ',' struct_declarator
;

struct_declarator : direct_declarator
| ':' conditional_expression
| direct_declarator ':' conditional_expression
;

enum_specifier : ENUM '{' enumerator_list '}'
| ENUM IDENTIFIER '{' enumerator_list '}'
| ENUM IDENTIFIER
;

enumerator_list : enumerator
| enumerator_list ',' enumerator
;

enumerator : IDENTIFIER
| IDENTIFIER '=' conditional_expression
;

direct_declarator : IDENTIFIER
| '(' direct_declarator ')'
| direct_declarator '[' conditional_expression ']'
| direct_declarator '[' ']'
| direct_declarator '(' parameter_list ')'
| direct_declarator '(' identifier_list ')'
| direct_declarator '(' ')'
;

parameter_list : parameter_declaration
| parameter_list ',' parameter_declaration
;

parameter_declaration : declaration_specifiers direct_declarator
| declaration_specifiers direct_abstract_declarator
| declaration_specifiers
;

identifier_list : IDENTIFIER
| identifier_list ',' IDENTIFIER
;

type_name : specifier_qualifier_list
| specifier_qualifier_list direct_abstract_declarator
;

direct_abstract_declarator : '(' direct_abstract_declarator ')'
| '[' ']'
| '[' conditional_expression ']'
| direct_abstract_declarator '[' ']'
| direct_abstract_declarator '[' conditional_expression ']'
| '(' ')'
| '(' parameter_list ')'
| direct_abstract_declarator '(' ')'
| direct_abstract_declarator '(' parameter_list ')'
;

initializer : assignment_expression
| '{' initializer_list '}'
;

initializer_list : initializer
| initializer_list ',' initializer
;

statement : labeled_statement
| compound_statement
| expression_statement
| selection_statement
| iteration_statement
| jump_statement
;

labeled_statement : CASE conditional_expression ':' statement
| DEFAULT ':' statement
;

compound_statement : '{' '}'
| '{' statement_list '}'
| '{' declaration_list '}'
| '{' declaration_list statement_list '}'
;

declaration_list : declaration
| declaration_list declaration
;

statement_list : statement
| statement_list statement
;

expression_statement : ';'
| expression ';'
;

selection_statement : IF '(' expression ')' statement
| IF '(' expression ')' statement ELSE statement
| SWITCH '(' expression ')' statement
;

iteration_statement : WHILE '(' expression ')' statement
| DO statement WHILE '(' expression ')' ';'
| FOR '(' expression_statement expression_statement ')' statement
| FOR '(' expression_statement expression_statement expression ')' statement
;

jump_statement : BREAK ';'
| RETURN ';'
| RETURN expression ';'
;

translation_unit : external_declaration
| translation_unit external_declaration
;
	
external_declaration : function_definition
| declaration
;

function_definition : declaration_specifiers direct_declarator declaration_list compound_statement
| declaration_specifiers direct_declarator compound_statement
| direct_declarator declaration_list compound_statement
| direct_declarator compound_statement
;

%%
#include <stdio.h>

extern char yytext[];
extern int column;

yyerror(s)
char *s;
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}