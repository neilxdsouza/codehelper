#ifndef POSTGRESQL_DATABASE_CODEGENERATORFACTORY_H
#define POSTGRESQL_DATABASE_CODEGENERATORFACTORY_H


#include "AbstractDataBaseCodeGeneratorFactory.h"
#include "AbstractDataBaseCodeGenerator.h"
#include "PostgreSQLCodeGenerator.h"


struct PostgreSQLCodeGeneratorFactory: public AbstractDataBaseCodeGeneratorFactory
{
	PostgreSQLCodeGeneratorFactory()
	{ }
	virtual AbstractDataBaseCodeGenerator *
		CreateCodeGenerator(TableInfoType * p_tabInfo,
				    std::string & p_output_dir_path)
	{
		return new PostgreSQLCodeGenerator(p_tabInfo,
						   p_output_dir_path);
	}
};

#endif /* POSTGRESQL_DATABASE_CODEGENERATORFACTORY_H */
