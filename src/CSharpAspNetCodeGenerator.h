#ifndef CSHARP_ASPNET_CODE_GENERATOR
#define CSHARP_ASPNET_CODE_GENERATOR

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "tree.h"
#include "AbstractCodeGenerator.h"
#include "AbstractDataBaseCodeGenerator.h"
#include "AbstractUIGenerator.h"

struct CSharpAspNetCodeGenerator: public AbstractCodeGenerator
{
public:
CSharpAspNetCodeGenerator(TableInfoType * p_TableInfoType_ptr,
			  AbstractDataBaseCodeGenerator * p_dbCodeGenerator,
			  AbstractUIGenerator * p_uiGenerator,
			  std::string & p_outputDirPrefix)
	: AbstractCodeGenerator(p_TableInfoType_ptr, p_dbCodeGenerator,
				p_uiGenerator,
				p_outputDirPrefix)
	{ }

	void GenerateCode(FILE * fptr);

	// helper functions used by main functions above
	void print_dp_params(FILE *fptr);

	void print_lower_fname(FILE * fptr);
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
	void print_ui(FILE * fptr);

	private:
	// The 3 functions below used to be public earlier
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
	CSharpAspNetCodeGenerator& operator=(const CSharpAspNetCodeGenerator&);
	CSharpAspNetCodeGenerator(const CSharpAspNetCodeGenerator&);
	protected:
	~CSharpAspNetCodeGenerator() { }
};
#endif /* CSHARP_ASPNET_CODE_GENERATOR */
