O 						[0-7]
D 						[0-9]
L 						[a-zA-Z_]
H 						[a-fA-F0-9]

%{
#include <stdio.h>
#include "y.tab.h"

void count();
%}

%%
"/*" 					{ comment(); }
"//"					{ comment_line(); }

"break" 				{ count(); return(BREAK); }
"case" 					{ count(); return(CASE); }
"char" 					{ count(); return(CHAR); }
"default" 				{ count(); return(DEFAULT); }
"do" 					{ count(); return(DO); }
"double" 				{ count(); return(DOUBLE); }
"else" 					{ count(); return(ELSE); }
"enum" 					{ count(); return(ENUM); }
"float" 				{ count(); return(FLOAT); }
"for" 					{ count(); return(FOR); }
"if" 					{ count(); return(IF); }
"int" 					{ count(); return(INT); }
"long" 					{ count(); return(LONG); }
"return" 				{ count(); return(RETURN); }
"short" 				{ count(); return(SHORT); }
"signed" 				{ count(); return(SIGNED); }
"struct" 				{ count(); return(STRUCT); }
"switch" 				{ count(); return(SWITCH); }
"typedef" 				{ count(); return(TYPEDEF); }
"unsigned" 				{ count(); return(UNSIGNED); }
"void" 					{ count(); return(VOID); }
"while" 				{ count(); return(WHILE); }

{L}({L}|{D})* 			{ count(); return(IDENTIFIER); }

0[xX]{H}+ 				{ count(); return(CONSTANT); }
0{O}+ 					{ count(); return(CONSTANT); }
{D}+ 					{ count(); return(CONSTANT); }
L?'(\\.|[^\\'])+' 		{ count(); return(CONSTANT); }

{D}*"."{D}+				{ count(); return(CONSTANT); }
{D}+"."{D}*				{ count(); return(CONSTANT); }

L?\"(\\.|[^\\"])*\" 	{ count(); return(STRING_LITERAL); }

"+="					{ count(); return(ADD_ASSIGN); }
"-=" 					{ count(); return(SUB_ASSIGN); }
"*="  					{ count(); return(MUL_ASSIGN); }
"/="  					{ count(); return(DIV_ASSIGN); }
"%="  					{ count(); return(MOD_ASSIGN); }
"++"  					{ count(); return(INC_OP); }
"--"  					{ count(); return(DEC_OP); }
"&&"  					{ count(); return(AND_OP); }
"||"  					{ count(); return(OR_OP); }
"<="  					{ count(); return(LE_OP); }
">="  					{ count(); return(GE_OP); }
"=="  					{ count(); return(EQ_OP); }
"!=" 					{ count(); return(NE_OP); }
";" 					{ count(); return(';'); }
"{" 					{ count(); return('{'); }
"}" 					{ count(); return('}'); }
","  					{ count(); return(','); }
":"  					{ count(); return(':'); }
"=" 					{ count(); return('='); }
"(" 					{ count(); return('('); }
")" 					{ count(); return(')'); }
"[" 					{ count(); return('['); }
"]" 					{ count(); return(']'); }
"!" 					{ count(); return('!'); }
"-" 					{ count(); return('-'); }
"+" 					{ count(); return('+'); }
"*" 					{ count(); return('*'); }
"/" 					{ count(); return('/'); }
"%" 					{ count(); return('%'); }
"<" 			 		{ count(); return('<'); }
">" 					{ count(); return('>'); }
"?"						{ count(); return('?'); }
"#"						{ count(); return('#'); }

[ \t\v\n\f] 			{ count(); }
. 						{ /* ignore bad characters */ }

%%

yywrap()
{
	return(1);
}


comment()
{
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0)
		putchar(c);

	if ((c1 = input()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}

	if (c != 0)
		putchar(c1);
}

comment_line()
{
	char c, c1;
	
loop:
	while ((c = input()) != '\n')
		putchar(c);
}


int column = 0;

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

	ECHO;
}