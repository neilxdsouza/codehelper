
#ifndef CPP_CODE_GENERATOR_FACTORY_H
#define CPP_CODE_GENERATOR_FACTORY_H

#include "tree.h"
#include "AbstractCodeGeneratorFactory.h"
#include "CppCodeGenerator.h"


struct CppCodeGeneratorFactory: public AbstractCodeGeneratorFactory
{
	CppCodeGeneratorFactory(AbstractDataBaseCodeGenerator * p_dbCodeGeneratorFactory)
		: AbstractCodeGeneratorFactory(p_dbCodeGeneratorFactory)
	{ }
	AbstractCodeGenerator* CreateCodeGenerator(TableInfoType * p_TableInfoType_ptr)
	{ return new CppCodeGenerator(p_TableInfoType_ptr, dbCodeGenerator_); }
};

#endif /* CPP_CODE_GENERATOR_FACTORY_H */
