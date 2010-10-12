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
	std::string GenerateUIScaffolding();
	std::string PrintHeaders();
	std::string PrintClassDecl();
	std::string PrintNavigationDecl();
	void AddVariableDecl(std::string & p_var_decl);
	void AddFunctionDefn(std::string & p_func_defn);
	void AddFunctionDecl(std::string & p_func_defn);
	void AddNavigationNode(std::string & label, std::string & func_name);

	std::stringstream class_vars;
	std::stringstream class_functions;
	std::stringstream class_function_impl;
	std::stringstream navigation_nodes;
};

#endif /* WT_UI_GENERATOR_H */
