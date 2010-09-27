#ifndef POSTGRESQL_CODEGENERATOR_H
#define POSTGRESQL_CODEGENERATOR_H

struct PostgreSQLCodeGenerator: public AbstractDataBaseCodeGenerator
{
	PostgreSQLCodeGenerator()
	{ }
	virtual void GenerateCode();
};

#endif /* POSTGRESQL_CODEGENERATOR_H */
