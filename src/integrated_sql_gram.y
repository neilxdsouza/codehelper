/*
 *  gram.y grammar for code generator
 *  Copyright (C) 2008, 2009 Neil Xavier D'Souza
 *  Released under GNU GPL Version 2
 */

%{
#include <iostream>
#include <vector>

#include "AbstractCodeGeneratorFactory.h"
#include "TableCollectionSingleton.hpp"
#include "symtab.h"
#include "scope.h"
#include "tree.h"
#include "stmt.h"
#include "global_variables.h"
#include "error.h"
//#include "stmt.h"

	extern int line_no;
	int no_errors=0;
	extern scope* active_scope;
	extern stmt * tree_root;
	extern vector <scope*> active_scope_list;
	template<class T> T* link_chain(T* & elem1, T* & elem2);
	// template<typename T> T* trav_chain(T* & elem1)
	template<class T> T* trav_chain(T* elem1);
	int lookup_func(string func_name_index);
	vector <TableInfoType*> table_info_table;
	int yylex();
	void yyerror(char * s);
	extern void yyrestart ( FILE *input_file );
	struct options_list_type options_list;
	struct tab_level_options_list_type tab_options;
	extern AbstractCodeGeneratorFactory * codeGeneratorFactory;
	std::vector<var_list *> vec_var_list;

	struct stmt * load_table_into_symbol_table( char * & name,  struct var_list* & v_list);

	multimap<string, vector<var_list*> > mm_field_groups;
	vector<var_list*> current_field_group;

%}

%union {
	double dval;
	int ival ;
	char * name;
	struct stmt * stmt;
	struct var_list * v_list;
	datatype dt;
};

%type <stmt> prog
%type <stmt>  statement
%type <stmt>  statement_list
%type <v_list>  var_decl_with_or_wo_options
%type <v_list>  var_decl
%type <v_list>  decl_comma_list
%type <v_list>  ui_field_groups
%type <v_list>  ui_field_group
%type <v_list>  opt_ui_field_groups
%type <dt> data_type


%token <dt> BIT_T
%token <dt> BIGINT_T
%token <dt> INT32_T
%token <dt> FLOAT_T
%token <dt> DOUBLE_T
%token <dt> VARCHAR_T
%token <dt> NVARCHAR_T
%token <dt> DATETIME_T
%token <dt> NTEXT_T
%token <dt> TEXT_T
%token <dt> TINYINT_T
%token <dt> IMAGE_T
%token <dt> NCHAR_T
%token <dt> COMPOSITE_T
%token CREATE REFERENCES MANY NOT DBNULL UNIQUE
%token VALIDATOR_REQ_FIELD VALIDATOR_RE_INTEGER VALIDATOR_RE_FLOAT VALIDATOR_RE_ALPHANUM VALIDATOR_RE_ALPHANUMWSP  SEARCH_KEY PRIMARY_KEY VISIBLE INVISIBLE UI_VIEW UI_SELECT EMBEDDED SESSION ARROW
%token UI_GROUP

%token TABLE
%token ','
%token ')'
%token '('
%token ';'
%token ':'
%token <name> NAME
%token <ival> INUMBER

%%

prog: statement_list{
		tree_root=trav_chain($1);
	}
    	;

statement_list: statement {
		$$=$1; 
	}
	| statement_list statement{
		$$=link_chain($1,$2);
	}
	;

statement:	CREATE TABLE NAME tab_level_options '(' opt_ui_field_groups ')' ';' {
		char *name=strdup($3);
		struct var_list* v_list=trav_chain($6);
		$$=new table_decl_stmt( TABLE_TYPE, line_no, name,  v_list, codeGeneratorFactory,
				vec_var_list, tab_options, mm_field_groups);
		vec_var_list.clear();
		global_variables::nGraphNodes++;
		typedef multimap<string, vector<var_list*> >::const_iterator mm_it_type;
		// cout << "Field Groups for : " << name << "are " << endl;
		//mm_it_type it = mm_field_groups.begin();
		//cout << (*it).first << endl;
		//for (mm_it_type mm_iter=mm_field_groups.begin(); 
		//		mm_iter!=mm_field_groups.end(); ++mm_iter) {
		//	//cout << mm_iter->first << ": ";
		//	cout << (*mm_iter).first << ": ";
		//	const vector <var_list*> & vv = (*mm_iter).second;
		//	for (int i=0; i<vv.size(); ++i) {
		//		cout << vv[i]->var_name << ", ";
		//	}
		//	cout << endl;
		//}
		mm_field_groups.clear();
	 }
	 ;

data_type:	INT32_T
	|	FLOAT_T
	|	BIGINT_T
	|	DOUBLE_T
	|	VARCHAR_T
	|	NVARCHAR_T
	|	NCHAR_T
	|	NTEXT_T
	|	TEXT_T
	|	BIT_T
	|	DATETIME_T
	|	TINYINT_T
	|	IMAGE_T
	|	COMPOSITE_T
	;

tab_level_options: 	/* empty */
	| ':' non_empty_tab_level_options
	;

non_empty_tab_level_options: tab_option
	|	non_empty_tab_level_options tab_option
	;

tab_option: UI_GROUP '(' NAME ')' {
		tab_options.setUIGroupName($3);
	}
	;

opt_ui_field_groups: decl_comma_list {
		$$ = $1;
	} 
	| ui_field_groups {
		$$ = $1;
	}
	;


ui_field_groups: ui_field_group {
		$$ = $1;
	}
	|	ui_field_groups ui_field_group {

		cout << __FILE__ << ", " << __LINE__ << " grammer rule: chaining ui_field_groups ui_field_group " << endl;
		cout << "chain $1: " << endl;
		struct var_list * temp = $1;
		while (temp) {
			cout << temp->var_name << " ";
			temp = temp->next;
		}
		cout << endl;
		cout << "chain $2: " << endl;
		temp = $2;
		while (temp) {
			cout << temp->var_name << " ";
			temp = temp->next;
		}
		cout << endl;

		// struct var_list * temp1 = trav_chain($2);
		// $$ = link_chain($1, temp1);
		$$ = link_chain($1, $2);
		//vec_var_list.push_back($3);
		cout << "after chaining $$: " << endl;
		temp = $$;
		while (temp) {
			cout << temp->var_name << " ";
			temp = temp->next;
		}
		cout << endl << endl;

	}
	;

ui_field_group:	NAME ARROW decl_comma_list {
		/*
		{
			struct var_list * temp = $3;
			cout << "BEFORE: " << __FILE__ << ", " << __LINE__ << " grammer rule: ui_field_group " << endl;
			while (temp) {
				cout << temp->var_name << " ";
				temp = temp->next;
			}
			cout << endl;
		}
		*/
		//struct var_list * vv_ptr = $3;
		struct var_list * head= trav_chain($3);
		struct var_list * vv_ptr = head;
		
		while (vv_ptr) {
			current_field_group.push_back(vv_ptr);
			vv_ptr = vv_ptr->prev;
		}
		string field_group_name($1);
		//if (mm_field_groups.find(field_group_name) == mm_field_groups.end()) {
			mm_field_groups.insert(make_pair(field_group_name, current_field_group));
		//}
		current_field_group.clear();
		//$$ = head;
		$$ = $3;
		/*
		{
			struct var_list * temp = $3;
			cout << "AFTER: " << __FILE__ << ", " << __LINE__ << " grammer rule: ui_field_group " << endl;
			while (temp) {
				cout << temp->var_name << " ";
				temp = temp->next;
			}
			cout << endl;
		}
		*/
	}
	;


decl_comma_list: var_decl_with_or_wo_options {
		$$ = $1;
		vec_var_list.push_back($1);
		//cout << "got decl_comma_list : " << endl;
	}
	| decl_comma_list ',' var_decl_with_or_wo_options {
		// cout << "decl_comma_list , var_decl_with_or_wo_options" << endl;
		// cout << "chain $1: " << endl;
		// struct var_list * temp = $1;
		// while (temp) {
		// 	cout << temp->var_name << " ";
		// 	temp = temp->next;
		// }
		// cout << endl;
		// cout << "chain $3: " << endl;
		// temp = $3;
		// while (temp) {
		// 	cout << temp->var_name << " ";
		// 	temp = temp->next;
		// }
		// cout << endl;
		$$ = link_chain($1, $3);
		vec_var_list.push_back($3);

		//cout << "chaining var_decl : " << endl;
	}
	;

var_decl_with_or_wo_options: var_decl ':' options_list {
	$$ = $1;
	//$1->options = options_list;
	$$->options = options_list;

	//cout << "var_name: " << $1->var_name << "options: unique = " 
	//	<< $1->options.unique << endl;
	options_list.reinit();
	}
	;

options_list: options
	| options_list  options
	;

options: REFERENCES NAME '(' NAME ')' {
		options_list.ref_field_name = $4;
		options_list.ref_table_name = $2;
		options_list.many = false;
	}
	|	REFERENCES MANY NAME '(' NAME ')' {
		options_list.ref_field_name = $5;
		options_list.ref_table_name = $3;
		options_list.many = true;
	}
	|	DBNULL {
		options_list.null = true;
		//cout << "parsing option DBNULL" ;
	}
	|	NOT DBNULL {
		/*
		$1->null=false;
		*/
		options_list.null = false;
	}
	|	UNIQUE {
		options_list.unique = true;
	}
	|	VALIDATOR_REQ_FIELD {
		options_list.validator = req_field;
	}
	|	VALIDATOR_RE_INTEGER {
		options_list.validator = re_int;
	}
	|	VALIDATOR_RE_FLOAT {
		options_list.validator = re_float;
	}
	|	VALIDATOR_RE_ALPHANUM {
		options_list.validator = re_alnum;
	}
	|	VALIDATOR_RE_ALPHANUMWSP {
		options_list.validator = re_alnumwsp;
	}
	|	SEARCH_KEY {
		options_list.search_key = true;
	}
	|	PRIMARY_KEY {
		options_list.primary_key = true;
	}
	|	VISIBLE {
		options_list.visible = true;
	}
	|	INVISIBLE {
		options_list.visible = false;
	}
	|	UI_SELECT {
		options_list.ui_select = true;
	}
	| 	UI_VIEW {
		options_list.ui_view = true;
	}
	|	EMBEDDED {
		options_list.embedded = true;
	}
	|	SESSION	{
		options_list.session = true;
	}
	;



var_decl: 	NAME data_type{
		$$=new var_list($2, $1);
	}
	| NAME data_type '(' INUMBER ')' {
		if(!($2==VARCHAR_TYPE||$2==NVARCHAR_TYPE ||$2==NCHAR_TYPE)){
			cerr << "only varchar allowed : line: " << line_no << endl;
			++no_errors;
		}
		$$=new var_list($2, $1, $4);
	}
	;

%%




template<typename T> T* link_chain(T* &elem1, T* &elem2)
{
	T * tail = elem2;
	//if (elem2->next)
	//	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "  ENTER ");	
	while (elem2->next) {
		//cout << ".";
		elem2 = elem2->next;
	}
	elem2->next=elem1;
	elem1->prev=elem2;
	return tail;
}

//template<typename T> T* trav_chain(T* & elem1)
template<typename T> T* trav_chain(T* elem1)
{
	while (elem1->next) elem1=elem1->next;
	return elem1;
}


