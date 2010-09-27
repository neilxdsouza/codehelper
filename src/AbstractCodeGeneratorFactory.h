#ifndef ABSTRACT_CODE_GENERATOR_FACTORY_H
#define ABSTRACT_CODE_GENERATOR_FACTORY_H

#include "AbstractCodeGenerator.h"
#include "AbstractDataBaseCodeGenerator.h"
#include "tree.h"

struct AbstractCodeGeneratorFactory
{
	AbstractDataBaseCodeGenerator * dbCodeGenerator_;
	AbstractCodeGeneratorFactory(AbstractDataBaseCodeGenerator * p_dbCodeGenerator)
	: dbCodeGenerator_(p_dbCodeGenerator)
	{ }
	//virtual void print_sql_provider(FILE *fptr)=0;
	//virtual void print_abstract_provider(FILE * fptr)=0;
	//virtual void print_sp(FILE * fptr)=0;
	//virtual void print_bll(FILE * fptr)=0;
	//virtual void print_ui(FILE * fptr)=0;
	//virtual void print(FILE * fptr)=0;
	virtual AbstractCodeGenerator* CreateCodeGenerator(TableInfoType * p_TableInfoType_ptr)=0;
	
	protected:
	~AbstractCodeGeneratorFactory()
	{ }
};

#endif /* ABSTRACT_CODE_GENERATOR_FACTORY_H */
