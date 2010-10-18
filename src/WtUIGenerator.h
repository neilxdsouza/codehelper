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
	void AddNavigationNode(std::string  label, std::string  func_name);
	void AddIncludeFile(std::string  p_include_file);
	string GenerateUIInsertForm();
	void GenerateUITab(std::stringstream & decl, std::stringstream & defn,
				bool called_recursively,
				vector<TableInfoType *> p_vecTableInfo);
	void GenerateForms();
	void GenerateMakefile();

	static std::stringstream class_vars;
	static std::stringstream class_functions_decl;
	static std::stringstream class_function_impl;
	static std::stringstream navigation_nodes;
	static std::stringstream header_files;
	static std::stringstream makefile_objs;

};

	//bool ReferencedTableContainsUs(TableInfoType *me, std::string ref_table_name);

#endif /* WT_UI_GENERATOR_H */
