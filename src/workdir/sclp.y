%{
	#include "function.h"
	#include <stack>

	#define YYSTYPE void*

	extern bool sa_parse;

	extern int yylex(void);

	extern char type;
	extern int int_val;
	extern double float_val;

	std::stack<AST_CompoundStatement*> comp_stack = {};

	void yyerror(const char* s) {
		while (comp_stack.size()) {delete comp_stack.top(); comp_stack.pop();}
		raise_error(s);
	}
%}

%token INT_NUM FLOAT_NUM STR_CONST
%token VOID INTEGER BOOL FLOAT STRING
%token RETURN NAME

%token GT GE LT LE EQ NE NOT OR AND
%token READ WRITE

%token IF ELSE DO WHILE

%nonassoc IFX
%nonassoc ELSE

%right '?' ':'
%left OR
%left AND
%right NOT
%nonassoc GT GE LT LE EQ NE
%left '+' '-'
%left '*' '/'
%right UMIN

%%

program
	: global_decl_stmt_list func_def_list {validate_symbol_table(); build_and_print_IRs();}
	| func_def_list {validate_symbol_table(); build_and_print_IRs();}
;

global_decl_stmt_list
	: global_decl_stmt_list func_decl
	| global_decl_stmt_list declaration_statement
	| func_decl
	| declaration_statement
;

func_def_list
	: func_def_list func_def
	| func_def
;

func_decl
	: func_header ';' {if (!sa_parse) curr_func -> validate_decl(); curr_func = nullptr;}
;

func_def
	: func_header '{' {if (!sa_parse) curr_func -> validate_def();} decl_statement_list statement_list '}' {curr_func = nullptr;}
;

func_header
	: named_type NAME {if (!sa_parse) curr_func = new Function(type, *(string*)$2); delete (string*)$2;} '(' optional_formal_param_list ')'
;

optional_formal_param_list
	: formal_param_list
	|
;

formal_param_list
	: formal_param_list ',' formal_param
	| formal_param
;

formal_param
	: param_type NAME {if (!sa_parse) curr_func -> formal_params.push_back(make_pair(*(string*)$2, type)); delete (string*)$2;}
;

param_type
	: INTEGER
	| FLOAT
	| BOOL
	| STRING
;

statement_list
	: statement_list statement {if (!sa_parse) comp_stack.size() ? comp_stack.top() -> stmt_list -> push_back((AST_Statement*)$2) : curr_func -> ast_statements -> push_back((AST_Statement*)$2);}
	|
;

statement
	: func_call ';'										{if (!sa_parse) $$ = new AST_CallStatement("", (AST_FuncExpression*)$1);}
	| READ NAME ';'										{if (!sa_parse) $$ = new AST_ReadStatement(*(string*)$2); delete (string*)$2;}
	| compound_statement								{if (!sa_parse) $$ = $1;}
	| WRITE expression ';'								{if (!sa_parse) $$ = new AST_PrintStatement((AST_Expression*)$2);}
	| RETURN expression ';'								{if (!sa_parse) $$ = new AST_ReturnStatement((AST_Expression*)$2);}
	| NAME '=' func_call ';'							{if (!sa_parse) $$ = new AST_CallStatement(*(string*)$1, (AST_FuncExpression*)$3); delete (string*)$1;}
	| NAME '=' expression ';'							{if (!sa_parse) $$ = new AST_AssignStatement(*(string*)$1, (AST_Expression*)$3); delete (string*)$1;}
	| WHILE '(' expression ')' statement				{if (!sa_parse) $$ = new AST_WhileStatement((AST_Expression*)$3, (AST_Statement*)$5);}
	| DO statement WHILE '(' expression ')' ';'			{if (!sa_parse) $$ = new AST_DoWhileStatement((AST_Expression*)$5, (AST_Statement*)$2);}
	| IF '(' expression ')' statement %prec IFX			{if (!sa_parse) $$ = new AST_IfElseStatement((AST_Expression*)$3, (AST_Statement*)$5, nullptr);}
	| IF '(' expression ')' statement ELSE statement	{if (!sa_parse) $$ = new AST_IfElseStatement((AST_Expression*)$3, (AST_Statement*)$5, (AST_Statement*)$7);}
;

compound_statement
	: '{' {if (!sa_parse) comp_stack.push(new AST_CompoundStatement());} statement_list '}' {if (!sa_parse) {$$ = comp_stack.top(); comp_stack.pop();}}
;

func_call
	: NAME '(' ')'			{if (!sa_parse) $$ = new AST_FuncExpression(*(string*)$1, new vector<AST_Expression*>()); delete (string*)$1;}
	| NAME '(' arg_list ')'	{if (!sa_parse) $$ = new AST_FuncExpression(*(string*)$1, (vector<AST_Expression*>*)$3); delete (string*)$1;}
;

arg_list
	: expression				{if (!sa_parse) $$ = new vector<AST_Expression*>({(AST_Expression*)$1});}
	| arg_list ',' expression	{if (!sa_parse) {((vector<AST_Expression*>*)$1) -> push_back((AST_Expression*)$3); $$ = $1;}}
;

decl_statement_list
	: decl_statement_list declaration_statement
	|
;

declaration_statement
	: named_type var_decl_item_list ';'
;

var_decl_item_list
	: NAME							{add_decl(type, *(string*)$1); delete (string*)$1;}			
	| var_decl_item_list ',' NAME	{add_decl(type, *(string*)$3); delete (string*)$3;}
;

named_type
	: INTEGER
	| FLOAT
	| BOOL
	| STRING
	| VOID
;

expression
	: var_and_consts
	| rel_expression
	| bool_expression
	| arith_expression
	| ternary_expression
	| '(' expression ')'	{if(!sa_parse) $$ = $2;}
;

var_and_consts
	: NAME		{if (!sa_parse) $$ = new AST_VarExpression(*(string*)$1); delete (string*)$1;}
	| INT_NUM	{if (!sa_parse) $$ = new AST_IntExpression(int_val);}
	| FLOAT_NUM	{if (!sa_parse) $$ = new AST_FloatExpression(float_val);}
	| STR_CONST	{if (!sa_parse) {$$ = new AST_StringExpression(*(string*)$1); add_string(*(string*)$1);} delete (string*)$1;}
;

arith_expression
	: '-' expression %prec UMIN	{if (!sa_parse) $$ = new AST_UnaryExpression("Uminus", (AST_Expression*)$2);}
	| expression '/' expression	{if (!sa_parse) $$ = new AST_BinaryExpression("Div", (AST_Expression*)$1, (AST_Expression*)$3, 'a');}
	| expression '+' expression	{if (!sa_parse) $$ = new AST_BinaryExpression("Plus", (AST_Expression*)$1, (AST_Expression*)$3, 'a');}
	| expression '*' expression	{if (!sa_parse) $$ = new AST_BinaryExpression("Mult", (AST_Expression*)$1, (AST_Expression*)$3, 'a');}
	| expression '-' expression	{if (!sa_parse) $$ = new AST_BinaryExpression("Minus", (AST_Expression*)$1, (AST_Expression*)$3, 'a');}
;

rel_expression
	: expression GT expression	{if (!sa_parse) $$ = new AST_BinaryExpression("GT", (AST_Expression*)$1, (AST_Expression*)$3, 'r');}
	| expression GE expression	{if (!sa_parse) $$ = new AST_BinaryExpression("GE", (AST_Expression*)$1, (AST_Expression*)$3, 'r');}
	| expression LT expression	{if (!sa_parse) $$ = new AST_BinaryExpression("LT", (AST_Expression*)$1, (AST_Expression*)$3, 'r');}
	| expression LE expression	{if (!sa_parse) $$ = new AST_BinaryExpression("LE", (AST_Expression*)$1, (AST_Expression*)$3, 'r');}
	| expression EQ expression	{if (!sa_parse) $$ = new AST_BinaryExpression("EQ", (AST_Expression*)$1, (AST_Expression*)$3, 'r');}
	| expression NE expression	{if (!sa_parse) $$ = new AST_BinaryExpression("NE", (AST_Expression*)$1, (AST_Expression*)$3, 'r');}
;

bool_expression
	: NOT expression			{if (!sa_parse) $$ = new AST_UnaryExpression("NOT", (AST_Expression*)$2);}
	| expression OR expression	{if (!sa_parse) $$ = new AST_BinaryExpression("OR", (AST_Expression*)$1, (AST_Expression*)$3, 'b');}
	| expression AND expression	{if (!sa_parse) $$ = new AST_BinaryExpression("AND", (AST_Expression*)$1, (AST_Expression*)$3, 'b');}
;

ternary_expression
	: expression '?' expression ':' expression	{if (!sa_parse) $$ = new AST_TernaryExpression((AST_Expression*)$1, (AST_Expression*)$3, (AST_Expression*)$5);}
;

%%
