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
		 AbstractDataBaseCodeGenerator * p_dbCodeGenerator)
	: AbstractCodeGenerator(p_TableInfoType_ptr, p_dbCodeGenerator)
	{ }

	void GenerateCode(FILE * fptr);
};

#endif /* CPP_CODE_GENERATOR_H */
