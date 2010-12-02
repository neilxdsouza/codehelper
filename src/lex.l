/*
 *  lex.l lexer for code generator
 *  Copyright (C) 2008, 2009 Neil Xavier D'Souza
 *  Released under GNU GPL Version 2
 */

%{
#define YY_NEVER_INTERACTIVE 1

#include "symtab.h"
#include "integrated_sql_gram.h"
#include <iostream>
	using std::cout;
	using std::endl;
	int line_no=1;
	extern int no_errors;
	void yyerror(char * s);

%}


%%


\/\/.*	{
	// ignore Comment
}



[ \t\r]+	; // eat whitespace


[\n]	{
	++line_no;
}


TABLE 	{
	return TABLE;
}

CREATE 	{
	return CREATE;
}

REFERENCES {
	return REFERENCES;
}
NOT	{ return NOT; }
NULL	{ return DBNULL; }

MANY	{
	return MANY;
}

UNIQUE	{
	return UNIQUE;
}

VISIBLE {
	return VISIBLE;
}

INVISIBLE {
	return INVISIBLE;
}

VALIDATOR_REQ_FIELD {
	return VALIDATOR_REQ_FIELD;
}

VALIDATOR_RE_INTEGER {
	return VALIDATOR_RE_INTEGER;
}

VALIDATOR_RE_FLOAT {
	return VALIDATOR_RE_FLOAT;
}

VALIDATOR_RE_ALPHANUM {
	return VALIDATOR_RE_ALPHANUM;
}

VALIDATOR_RE_ALPHANUMWSP {
	return VALIDATOR_RE_ALPHANUMWSP;
}

SEARCH_KEY {
	return SEARCH_KEY;
}

PRIMARY_KEY {
	return PRIMARY_KEY;
}

UI_SELECT {
	return UI_SELECT;
}

UI_VIEW {
	return UI_VIEW;
}

UI_GROUP {
	return UI_GROUP;
}

EMBEDDED {
	return EMBEDDED;
}

SESSION {
	return SESSION;
}

UI_DIALOG_SELECT_XFER {
	return UI_DIALOG_SELECT_XFER;
}

PASSWORD {
	return PASSWORD;
}

IS_LOGIN_PAGE {
	return IS_LOGIN_PAGE;
}

IS_LOGIN_USERNAME_FIELD {
	return IS_LOGIN_USERNAME_FIELD;
}

IS_LOGIN_PASSWORD_FIELD {
	return IS_LOGIN_PASSWORD_FIELD;
}

IS_MASTER_TABLES_LIST {
	return IS_MASTER_TABLES_LIST;
}


")"	{
	return ')';
}

"("	{
	return '(';
}
","	{
	return ',';
}
";"	{
	return ';';
}

":"	{
	//cout << "got token :" << endl;
	return ':';
}

"->"	{
	return ARROW;
}


'.' 	{
	return yytext[0];
}

[0-9]+	{
	yylval.ival = atoi(yytext);
	return INUMBER;
}

DATETIME { yylval.dt=DATETIME_TYPE; return DATETIME_T ; }
FLOAT	{ yylval.dt=FLOAT_TYPE; return FLOAT_T; }

DOUBLE { yylval.dt=DOUBLE_TYPE; return DOUBLE_T; }

INT		{ yylval.dt=INT32_TYPE;	       return INT32_T; }
BIT	{ yylval.dt=BIT_TYPE;	     return BIT_T; } 
BIGINT		{ yylval.dt=BIGINT_TYPE;	     return BIGINT_T; } 
NTEXT		{ yylval.dt=NTEXT_TYPE;	     return NTEXT_T; } 
TEXT		{ yylval.dt=TEXT_TYPE;	     return TEXT_T; } 
TINYINT	{ yylval.dt=TINYINT_TYPE;	     return TINYINT_T; }
VARCHAR	{ yylval.dt=VARCHAR_TYPE; return VARCHAR_T; }
NVARCHAR	{ yylval.dt=NVARCHAR_TYPE; return NVARCHAR_T; }
NCHAR	{ yylval.dt=NCHAR_TYPE; printf("got token nchar\n"); return NCHAR_T; }
IMAGE	{ yylval.dt=IMAGE_TYPE; return IMAGE_T; }
COMPOSITE	{ yylval.dt=COMPOSITE_TYPE; return COMPOSITE_T; }


[A-Za-z_][A-Za-z0-9_]*	{
	yylval.name=strdup(yytext);
	return NAME;
	}

%%


void yyerror(char * s){
	++no_errors;
	printf("%s: line: %d: yytext: %s\n", s, line_no, yytext  );
	printf("no_errors: %d\n", no_errors);
}

	int yywrap(){
		return 1;
	}
