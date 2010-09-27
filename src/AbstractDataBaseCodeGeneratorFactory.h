#ifndef ABSTRACT_DATABASE_CODEGENERATORFACTORY_H
#define ABSTRACT_DATABASE_CODEGENERATORFACTORY_H

#include "AbstractDataBaseCodeGenerator.h"

struct AbstractDataBaseCodeGeneratorFactory
{
	AbstractDataBaseCodeGeneratorFactory()
	{ }
	virtual AbstractDataBaseCodeGenerator * CreateCodeGenerator()=0;
};

#endif /* ABSTRACT_DATABASE_CODEGENERATORFACTORY_H */
