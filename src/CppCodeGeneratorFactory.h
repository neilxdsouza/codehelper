
#ifndef CPP_CODE_GENERATOR_FACTORY_H
#define CPP_CODE_GENERATOR_FACTORY_H

#include "tree.h"
#include "AbstractCodeGeneratorFactory.h"
#include "CppCodeGenerator.h"


struct CppCodeGeneratorFactory: public AbstractCodeGeneratorFactory
{
	CppCodeGeneratorFactory(AbstractDataBaseCodeGeneratorFactory *
				p_dbCodeGeneratorFactory, std::string&
				p_outputCodeDirectoryPrefix)
		: AbstractCodeGeneratorFactory(p_dbCodeGeneratorFactory,
			p_outputCodeDirectoryPrefix)
	{ }
	AbstractCodeGenerator* CreateCodeGenerator(TableInfoType * p_TableInfoType_ptr)
	{
		AbstractDataBaseCodeGenerator * db_code_generator=
			dbCodeGeneratorFactory_->
			CreateCodeGenerator(p_TableInfoType_ptr,
					    outputCodeDirectoryPrefix_);
		return new CppCodeGenerator(p_TableInfoType_ptr,
					    db_code_generator,
					    outputCodeDirectoryPrefix_);
	}
};

#endif /* CPP_CODE_GENERATOR_FACTORY_H */
