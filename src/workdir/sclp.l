%{
	#include <iostream>
	#include <fstream>

	#define YYSTYPE void*

	#include "function.h"
	#include "parse.tab.h"

	#define YY_NO_UNPUT

	extern std::ofstream tok_output_file;
	extern bool show_tokens;

	void print_token(string token_name){
		if (!show_tokens) return;
		tok_output_file << "\tToken Name: " << token_name;
		tok_output_file << " \tLexeme: " << yytext;
		tok_output_file << " \t Lineno: " << yylineno << std::endl;
	}

	char type;
	int int_val;
	double float_val;
%}

%option noyywrap
%option yylineno

%%

[ \t\n]
\/\/[^\n]*
[0-9]+							{print_token("INT_NUM"); int_val = atoi(yytext); return INT_NUM;}
[0-9]+\.[0-9]*					{print_token("FLOAT_NUM"); float_val = double(strtod(yytext, NULL)); return FLOAT_NUM;}
int								{print_token("INTEGER"); type = 'i'; return INTEGER;}
float							{print_token("FLOAT"); type = 'f'; return FLOAT;}
string							{print_token("STRING"); type = 's'; return STRING;}
bool							{print_token("BOOL"); type = 'b'; return BOOL;}
void							{print_token("VOID"); type = 'v'; return VOID;}
return							{print_token("RETURN"); return RETURN;}
print							{print_token("WRITE"); return WRITE;}
read							{print_token("READ"); return READ;}
if								{print_token("IF"); return IF;}
else							{print_token("ELSE"); return ELSE;}
do								{print_token("DO"); return DO;}
while							{print_token("WHILE"); return WHILE;}
==								{print_token("EQUAL"); return EQ;}
=								{print_token("ASSIGN_OP"); return '=';}
\!=								{print_token("NOT_EQUAL"); return NE;}
\!								{print_token("NOT"); return NOT;}
>=								{print_token("GREATER_THAN_EQUAL"); return GE;}
\<=								{print_token("LESS_THAN_EQUAL"); return LE;}
>								{print_token("GREATER_THAN"); return GT;}
\<								{print_token("LESS_THAN"); return LT;}
\|\|							{print_token("OR"); return OR;}
\&\&							{print_token("AND"); return AND;}
\?								{print_token("QUESTION_MARK"); return '?';}
:								{print_token("COLON"); return ':';}
\+								{print_token("PLUS"); return '+';}
\-								{print_token("MINUS"); return '-';}
\*								{print_token("MULT"); return '*';}
\/								{print_token("DIV"); return '/';}
\(								{print_token("LEFT_ROUND_BRACKET"); return '(';}
\)								{print_token("RIGHT_ROUND_BRACKET"); return ')';}
\{								{print_token("LEFT_CURLY_BRACKET"); return '{';}
\}								{print_token("RIGHT_CURLY_BRACKET"); return '}';}
;								{print_token("SEMICOLON"); return ';';}
,								{print_token("COMMA"); return ',';}
[a-zA-Z_][a-zA-Z0-9_]*			{print_token("NAME"); string tmp = string(yytext); if (tmp != "main") tmp += '_'; yylval = new string(tmp); return NAME;}
\"[^\"]*\"						{print_token("STR_CONST"); yylval = new string(yytext); return STR_CONST;}
.								{raise_error("Illegal Character '", yytext, "'");}

%%
