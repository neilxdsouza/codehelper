#ifndef CPP_CODE_GENERATOR_H
#define CPP_CODE_GENERATOR_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "tree.h"
#include "AbstractCodeGenerator.h"

struct CppCodeGenerator: public AbstractCodeGenerator
{
public:
	stringstream h_header, h_body, h_footer;
	stringstream cpp_body;

public:
CppCodeGenerator(TableInfoType * p_TableInfoType_ptr,
		 AbstractDataBaseCodeGenerator * p_dbCodeGenerator,
		 AbstractUIGenerator * p_uiGenerator,
		 std::string & p_outputDirPrefix)
	: AbstractCodeGenerator(p_TableInfoType_ptr, p_dbCodeGenerator,
				p_uiGenerator, 
				p_outputDirPrefix),
	  h_header(), h_body(), h_footer(),
	  cpp_body()
	{ }

	void GenerateCode(FILE * fptr);
	void FinalCleanUp();
	void GenerateBLL();
	void GenerateBLLDefn_h(std::ofstream & bll_h);
	void GenerateBLLDefn_cpp(std::ofstream & bll_cpp);
public:
	void print_bll_h_header(std::ofstream & bll_h);
	void print_bll_api_decls(std::ofstream & bll_h);
	void print_bll_h_footer(std::ofstream & bll_h);
	void print_bll_params(std::ofstream & bll_h);
	void print_bll_api(std::ofstream & bll_cpp);
	void print_bll_api_functions_decl(std::ofstream & bll_h);
	void print_bll_api_defns(std::ofstream & bll_cpp);
	void print_bll_api_test_stubs(std::ofstream & bll_cpp);
	void print_bll_api_constructors();
	void print_bll_constructor_decl_with_all_fields();
	void print_bll_constructor_decl_without_invisible_fields();
public:
	void print_bll_Insert_defn(std::ofstream & bll_cpp);
	void print_bll_Constructor_defn(std::ofstream & bll_cpp);
	void print_bll_Constructor_with_all_fields();
	void print_bll_Constructor_without_invisible_fields();
private:
	CppCodeGenerator(const CppCodeGenerator&);
	CppCodeGenerator& operator=(CppCodeGenerator &);
protected:
	~CppCodeGenerator()
	{ }

};
//	bool ReferencedTableContainsUs(TableInfoType * me, std::string ref_table_name);

#endif /* CPP_CODE_GENERATOR_H */
