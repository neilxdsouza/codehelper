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
CppCodeGenerator(TableInfoType * p_TableInfoType_ptr,
		 AbstractDataBaseCodeGenerator * p_dbCodeGenerator,
		 std::string & p_outputDirPrefix)
	: AbstractCodeGenerator(p_TableInfoType_ptr, p_dbCodeGenerator,
				p_outputDirPrefix)
	{ }

	void GenerateCode(FILE * fptr);
	void GenerateBLL();
	void GenerateBLLDefn_h(std::ofstream & bll_h);
	void GenerateBLLDefn_cpp(std::ofstream & bll_cpp);
public:
	void print_bll_h_header(std::ofstream & bll_h);
	void print_bll_api_decls(std::ofstream & bll_h);
	void print_bll_h_footer(std::ofstream & bll_h);
	void print_bll_params(std::ofstream & bll_h);
	void print_bll_api(std::ofstream & bll_cpp);
private:
	CppCodeGenerator(const CppCodeGenerator&);
	CppCodeGenerator& operator=(CppCodeGenerator &);

};

#endif /* CPP_CODE_GENERATOR_H */
