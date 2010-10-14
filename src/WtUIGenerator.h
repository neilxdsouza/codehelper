#ifndef  WT_UI_GENERATOR_H
#define WT_UI_GENERATOR_H

#include <sstream>

#include "AbstractUIGenerator.h"

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
	string GenerateUIInsertForm();
	void GenerateForms();

	static std::stringstream class_vars;
	static std::stringstream class_functions_decl;
	static std::stringstream class_function_impl;
	static std::stringstream navigation_nodes;
};

#endif /* WT_UI_GENERATOR_H */
