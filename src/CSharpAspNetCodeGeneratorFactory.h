#ifndef CSHARPASPNET_CODE_GENERATOR_FACTORY_H
#define CSHARPASPNET_CODE_GENERATOR_FACTORY_H

#include "tree.h"
#include "AbstractCodeGeneratorFactory.h"
#include "CSharpAspNetCodeGenerator.h"


struct CSharpAspnetCodeGeneratorFactory: public AbstractCodeGeneratorFactory
{
	CSharpAspnetCodeGeneratorFactory()
		: AbstractCodeGeneratorFactory()
	{ }
	AbstractCodeGenerator* CreateCodeGenerator(TableInfoType * p_TableInfoType_ptr)
	{ return new CSharpAspNetCodeGenerator(p_TableInfoType_ptr); }
};

#endif /* CSHARPASPNET_CODE_GENERATOR_FACTORY_H */
