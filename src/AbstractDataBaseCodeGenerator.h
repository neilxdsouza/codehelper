#ifndef ABSTRACT_DATABASE_CODEGENERATOR_H
#define ABSTRACT_DATABASE_CODEGENERATOR_H

struct AbstractDataBaseCodeGenerator
{
	AbstractDataBaseCodeGenerator()
	{ }
	virtual void GenerateCode()=0;

};

#endif /* ABSTRACT_DATABASE_CODEGENERATOR_H */
