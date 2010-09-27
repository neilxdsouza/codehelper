#ifndef POSTGRESQL_DATABASE_CODEGENERATORFACTORY_H
#define POSTGRESQL_DATABASE_CODEGENERATORFACTORY_H

#include "AbstractDataBaseCodeGeneratorFactory.h"
#include "PostgreSQLCodeGenerator.h"


struct PostgreSQLCodeGeneratorFactory: public AbstractDataBaseCodeGeneratorFactory
{
	PostgreSQLCodeGeneratorFactory()
	{ }
	virtual AbstractDataBaseCodeGenerator * CreateCodeGenerator()
	{
		return new PostgreSQLCodeGenerator();
	}
};

#endif /* POSTGRESQL_DATABASE_CODEGENERATORFACTORY_H */
