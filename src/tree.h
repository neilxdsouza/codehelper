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

struct options_list_type {
	string ref_table_name;
	string ref_field_name;
	bool many;
	bool null;
	bool unique;
	bool search_key;
	validator_types validator;

	options_list_type(string l_ref_table_name, string l_ref_field_name, 
			  bool l_many, bool l_null, bool l_unique, bool l_search_key, validator_types l_validator)
		: ref_table_name(l_ref_table_name), ref_field_name(l_ref_field_name),
		  many(l_many), null(l_null), unique(l_unique), search_key(l_search_key),
		  validator(l_validator)
	{ }

	options_list_type()
		: ref_table_name(), ref_field_name(),
		  many(false), null(false), unique(false), search_key(false),
		  validator(none)
	{ }
			   
	public:
	void reinit(){
		ref_table_name="";
		ref_field_name="";
		many=false;
		null=true;
		unique=false;
		validator=none;
		search_key = false;
	}
};

using namespace std;
void print_csharp_types(FILE * fptr, datatype dt);
void print_sp_types(FILE * fptr, datatype dt);
void print_csharp_db_types(FILE * fptr, datatype dt);
void print_aspx_types(FILE * fptr, datatype dt);
bool isOfStringType( datatype dt);
struct var_list {
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
struct table_info{
	public:
	enum print_sp_params_mode { SELECT, INSERT, UPDATE, DELETE };
	string fname;
	struct var_list * param_list;
	struct scope * table_scope;
	int has_composite_objs;
	int has_multi;
	int has_search_key;
	void print_lower_fname(FILE * fptr){
		char buffer[MAX_VAR_LEN];
		if(fname.length() < MAX_VAR_LEN-1){
			strcpy(buffer, fname.c_str());
			buffer[0]=tolower(buffer[0]);
			fprintf(fptr, "%s", buffer);
		}
	}
	table_info(string name, struct var_list*  elist): 
		fname(name), param_list(elist), table_scope(0), has_composite_objs(0),
		has_multi(0), has_search_key(0){
		
		struct var_list* v_ptr = param_list;
		while(v_ptr){
			if(v_ptr->options.ref_table_name != ""){
				++has_composite_objs;
				if(v_ptr->options.many){
					++has_multi;
				}
			}
			if(v_ptr->options.search_key){
				++has_search_key;
			}
			v_ptr=v_ptr->prev;
		}
	}
	// main functions which print out an entire class
	//void print_detail(FILE * fptr);


	void print(FILE * fptr);

	// helper functions used by main functions above
	void print_dp_params(FILE *fptr);
	// sql provider
	public:
	void print_sql_provider(FILE *fptr);
	private:
	void print_sql_provider_get_index_size(FILE * fptr);
	void print_sql_provider_header(FILE* fptr);
	void print_sql_provider_get_count(FILE * fptr);
	void print_sql_provider_insert(FILE * fptr);
	void print_sql_provider_delete(FILE * fptr);
	void print_sql_provider_update(FILE * fptr);
	void print_sql_provider_get_by_code(FILE * fptr);
	public:
	void print_abstract_provider(FILE * fptr);
	private:
	//void print_reader(FILE * edit_out);
	void print_reader(FILE * edit_out, bool with_pkey, bool rename_vars, string inner_join_tabname);
	void print_get_collection(FILE * fptr);
	void print_get_single_record(FILE * fptr);
	void print_insert_dp_params(FILE *fptr);
	// SQL procedures printer
	public:
	void print_sp(FILE * fptr);
	private:
	void print_sp_select(FILE * fptr);
	void print_sp_insert(FILE * fptr);
	void print_sp_update(FILE * fptr);
	void print_sp_delete(FILE * fptr);
	void print_sp_header(FILE * fptr);
	void print_sp_params(FILE * fptr, print_sp_params_mode mode);
	void print_sp_select_by_pkey(FILE * fptr);
	void print_sp_count(FILE* fptr);	
	void print_sp_select_params(FILE* fptr, bool with_pkey, bool rename_vars, string inner_join_tabname);
	void print_sp_select_fields(FILE* fptr);
	void print_reader_param_with_cast(FILE* fptr, var_list* v_ptr);
	void print_sp_unique_checks(FILE* fptr);
	void print_sp_search_key_params(FILE* fptr, bool print_comma);
	void print_sp_search_key_whereclause(FILE* fptr);
	public:
	// helper functions for printing the BLL
	void print_bll(FILE * fptr);
	private:
	void print_bll_api(FILE * fptr);
	void print_bll_constructor(FILE * fptr);
	void print_bll_bizobj_constructor(FILE * fptr);
	void print_bll_constructors(FILE * fptr);
	void print_bll_getcount(FILE* fptr);
	void print_bll_get_collection(FILE* fptr);
	void print_bll_get_paged_collection(FILE* fptr);
	void print_bll_get_single_entity(FILE* fptr);
	void print_bll_insert_entity(FILE * fptr);
	void print_bll_update_entity(FILE* fptr);
	void print_bll_params(FILE * fptr);

	void print_func_params_without_pkey(FILE* fptr);
	void print_func_params_with_pkey(FILE* fptr);


	void print_bll_func_call_params(FILE* fptr);
	public:

	void print_aspx(FILE *fptr, bool called_recursively);
	void print_aspx_multi_view(FILE *fptr);
	void print_aspx_cs(FILE *fptr);
	private:
	void print_aspx_params(FILE* fptr);
	void print_bound_fields(FILE* fptr,struct var_list* start);
	void print_aspx_objds_many(FILE* fptr);
	void print_aspx_objds_single(FILE* fptr);
	void print_aspx_formview(FILE* fptr);
	void print_aspx_gridview(FILE* fptr);
	void print_aspx_header(FILE* fptr);
	void print_aspx_footer(FILE* fptr);
	void print_aspx_validator(FILE* fptr, struct var_list* v_ptr);
	void print_search_facility_aspx(FILE* fptr);
	void print_search_facility_aspx_cs(FILE* fptr);

	void print_ascx(FILE* fptr);
	void print_ascx_cs(FILE* fptr);
	bool print_registered_controls(FILE* fptr);
	void print_aspx_edit_fields(FILE* fptr, struct var_list* param_list);
	void print_aspx_insert_fields(FILE* fptr, struct var_list* param_list);
	void print_uc_event_registrations(FILE* fptr, struct var_list* param_list);
	void print_uc_event_handlers(FILE* fptr, struct var_list* param_list);
	void print_aspx_cs_header(FILE* fptr);
	void print_aspx_cs_body(FILE* fptr, bool is_a_many_table);
	void print_aspx_cs_footer(FILE* fptr);

	public: 
	void print_popup_aspx(FILE * fptr);
	void print_popup_aspx_cs(FILE * fptr);
	private:
	void print_popup_aspx_header( FILE * fptr);

	private:
	table_info& operator=(const table_info&);
	table_info(const table_info&);
};

#endif /* __NxD_TREE_H */
