#ifndef  WT_UI_GENERATOR_H
#define WT_UI_GENERATOR_H

#include <sstream>

#include "AbstractUIGenerator.h"

struct TableInfoType;

struct WtUIGenerator : public AbstractUIGenerator
{
public:
	WtUIGenerator(TableInfoType * p_tabInfo,
				std::string & p_output_dir_path);
	virtual void GenerateCode();
	virtual void FinalCleanUp();
	std::string GenerateUIScaffolding();
	std::string PrintHeaders();
	std::string PrintClassDecl();
	std::string PrintNavigationDecl();
	void AddVariableDecl(std::string  p_var_decl);
	void AddFunctionDefn(std::string  p_func_defn);
	void AddFunctionDecl(std::string  p_func_decl);

	void AddFunctionDefnToStream(std::string  p_func_defn, stringstream & defn);
	void AddFunctionDeclToStream(std::string  p_func_decl, stringstream & decl);
	void AddNavigationNode(std::string  label, std::string  func_name);
	void AddIncludeFile(std::string  p_include_file);
	string GenerateUIInsertForm();
	void GenerateUITab(std::stringstream & headers, 
				std::stringstream & decl, std::stringstream & defn,
				bool called_recursively,
				vector<TableInfoType *> p_vecTableInfo);
	void GenerateForms();
	void GenerateMakefile();
	string print_ChoiceHandler(struct var_list * p_vptr, std::stringstream & decl);
	string print_XferFunction(struct var_list * p_vptr, std::stringstream & decl);

	static std::stringstream class_vars;
	static std::stringstream class_functions_decl;
	static std::stringstream class_function_impl;
	static std::stringstream navigation_nodes;
	static std::stringstream header_files;
	static std::stringstream makefile_objs;
	std::vector<std::string> vec_handler_decls;
	std::vector<std::string> vec_handler_defns;
	std::string print_cpp_search_key_args();

	std::string PrintProcessInsert();
	std::string PrintUINavigation();
	std::string PrintUIMenu();

	void PrintListViewHeaders(stringstream  & p_load_table_view_str, vector<TableInfoType* > & p_vec_list_view_stack
			, int max_recursion_level, int recursion_level, int & nColumns);

	void PrintListViewData(stringstream  & p_load_table_view_str, vector<TableInfoType* > & p_vec_list_view_stack
			, int max_recursion_level, int recursion_level, int & nColumns);
};

	//bool ReferencedTableContainsUs(TableInfoType *me, std::string ref_table_name);

#endif /* WT_UI_GENERATOR_H */
