
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BIT_T = 258,
     BIGINT_T = 259,
     INT32_T = 260,
     FLOAT_T = 261,
     DOUBLE_T = 262,
     VARCHAR_T = 263,
     NVARCHAR_T = 264,
     DATETIME_T = 265,
     NTEXT_T = 266,
     TEXT_T = 267,
     TINYINT_T = 268,
     IMAGE_T = 269,
     NCHAR_T = 270,
     COMPOSITE_T = 271,
     CREATE = 272,
     REFERENCES = 273,
     MANY = 274,
     NOT = 275,
     DBNULL = 276,
     UNIQUE = 277,
     VALIDATOR_REQ_FIELD = 278,
     VALIDATOR_RE_INTEGER = 279,
     VALIDATOR_RE_FLOAT = 280,
     VALIDATOR_RE_ALPHANUM = 281,
     VALIDATOR_RE_ALPHANUMWSP = 282,
     SEARCH_KEY = 283,
     PRIMARY_KEY = 284,
     VISIBLE = 285,
     INVISIBLE = 286,
     UI_VIEW = 287,
     UI_SELECT = 288,
     EMBEDDED = 289,
     SESSION = 290,
     ARROW = 291,
     UI_DIALOG_SELECT_XFER = 292,
     PASSWORD = 293,
     IS_LOGIN_USERNAME_FIELD = 294,
     IS_LOGIN_PASSWORD_FIELD = 295,
     UI_GROUP = 296,
     IS_LOGIN_PAGE = 297,
     IS_MASTER_TABLES_LIST = 298,
     IS_ROLE_TABLE = 299,
     IS_USER_ROLES_TABLE = 300,
     TABLE = 301,
     NAME = 302,
     INUMBER = 303
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 48 "src/integrated_sql_gram.y"

	double dval;
	int ival ;
	char * name;
	struct stmt * stmt;
	struct var_list * v_list;
	datatype dt;



/* Line 1676 of yacc.c  */
#line 111 "src/integrated_sql_gram.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


