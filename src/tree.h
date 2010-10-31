/*
 *  tree.h : data structures for parsing trees 
 *  Copyright (C) 2008, 2009 Neil Xavier D'Souza
 *  Released under GNU GPL Version 2
 */




#ifndef __NxD_TREE_H
#define __NxD_TREE_H

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "my_defs.h"
#include "symtab.h"

datatype lcm_type(datatype d1, datatype d2);
//struct expr;

datatype arr_deref_type(datatype d1);

//struct expr * new_expr();

#include <string>
#include <iostream>
#include <cctype>
using std::string;

enum validator_types { none, req_field, re_int, re_float, re_alnum, re_alnumwsp };

struct options_list_type 
{
	string ref_table_name;
	string ref_field_name;
	bool many;
	bool null;
	bool unique;
	bool search_key;
	validator_types validator;
	bool primary_key;
	bool visible;

	options_list_type(string l_ref_table_name, string l_ref_field_name, 
			  bool l_many, bool l_null, bool l_unique, bool l_search_key, 
			  validator_types l_validator, bool l_primary_key, bool l_visible)
		: ref_table_name(l_ref_table_name), ref_field_name(l_ref_field_name),
		  many(l_many), null(l_null), unique(l_unique), search_key(l_search_key),
		  validator(l_validator), primary_key(l_primary_key), visible(l_visible)
	{ }

	options_list_type()
		: ref_table_name(), ref_field_name(),
		  many(false), null(false), unique(false), search_key(false),
		  validator(none), primary_key(false), visible(true)
	{ }
			   
	public:
	void reinit()
	{
		ref_table_name="";
		ref_field_name="";
		many=false;
		null=true;
		unique=false;
		validator=none;
		search_key = false;
		primary_key=false;
		visible=true;
	}
};

using namespace std;
//void print_csharp_types(FILE * fptr, datatype dt);
//void print_sp_types(FILE * fptr, datatype dt);
//void print_csharp_db_types(FILE * fptr, datatype dt);
//void print_aspx_types(FILE * fptr, datatype dt);
//bool isOfStringType( datatype dt);
struct var_list 
{
	datatype var_type;
	string var_name;
	int arr_len;
	
	struct options_list_type options;

	/*
	string ref_table_name;
	string ref_field_name;
	bool many, null, unique, search_key;
	validator_types validator;
	*/
	struct var_list * prev;
	struct var_list * next;
	var_list(datatype type, char * name)
		: var_type(type), var_name(name), arr_len(-1),
		  /*
		  ref_table_name(), ref_field_name(), 
		  many(false), null(false), unique(false), search_key(false), 
		  */
		  options(),
		  prev(0), next(0) 
	{
		//ref_field_name="";
		//ref_table_name="";
		//many=false;
		//null=true;
		//unique=false;
		//validator = none;
		//search_key=false;
	}
	var_list(datatype type, char * name, int len)
		: var_type(type), var_name(name), arr_len(len),
		  /*
		  ref_table_name(), ref_field_name(), 
		  many(false), null(false), unique(false), search_key(false), 
		  */
		  options(),
		  prev(0), next(0)
	{
		//ref_field_name="";
		//ref_table_name="";
		//many=false;
		//null=true;
		//unique=false;
		//validator = none;
		//search_key=false;
	}
	void print_1st_var(FILE * edit_out) {
		char buff[MAX_VAR_LEN];
		if(var_name.length() < MAX_VAR_LEN-1){
			strcpy(buff, var_name.c_str());
			buff[0]=tolower(buff[0]);
			fprintf(edit_out, "%s", buff);
		} else {
			fprintf(edit_out, " VAR NAME TOO LONG: file:%s, line: %d, func: print_1st_var\n", 
					__FILE__, __LINE__);

		}
	}
	std::string print_cpp_var_type();
	std::string print_cpp_var_param_name();
	std::string print_cpp_var_name();
	std::string print_lower_var_name();
	std::string print_improved_lower_var_name();

	std::string print_sql_var_decl();
	std::string print_sql_var_type();
	std::string print_sql_var_decl_for_select_return_table();
	std::string print_sql_var_name_for_select_return_table(std::string ref_table_name);
	std::string print_random_value(int counter, int p_nRecords);
	// 30-oct-2010
	// move psql to cpp conversion from PostgreSQLCodeGenerator to here
	std::string print_psql_to_cpp_conversion(std::string ref_table_name);

	private:
		var_list& operator=(const var_list&);
		var_list(const var_list&);
	
};

#include <cstdlib>
/*
 * The func_info constructor adds the names of the function parameters into its scope.
 * When a compound statement is parsed : it checks for the flag_cmpd_stmt_is_a_func_body and
 * loads that into the active scope if set.
 * Note that flag_cmpd_stmt_is_a_func_body is initialized to -1 as the 1st function 
 * will be in index 0 of func_info_table vector.
 * Also lookup_func searches the func_info_table for the function name and returns -1 on failure
 * this is naturally compatible with the initial value of flag_cmpd_stmt_is_a_func_body
 * if the flag is not set -> we need to allocate a new scope - else we will crash
 */
#include "scope.h"
enum print_sp_params_mode { SELECT, INSERT, UPDATE, DELETE };
struct TableInfoType
{
	public:
	string tableName_;
	struct var_list * param_list;
	struct scope * table_scope;
	int has_composite_objs;
	int has_multi;
	int has_search_key;
	int nInvisible;
	vector<var_list*> vec_var_list;
	void print_lower_fname(FILE * fptr);
	TableInfoType(string name, struct var_list*  elist
			, vector<var_list*> & p_vec_var_list); 

	// main functions which print out an entire class
	//void print_detail(FILE * fptr);


	private:
	TableInfoType& operator=(const TableInfoType&);
	TableInfoType(const TableInfoType&);
};


#endif /* __NxD_TREE_H */
