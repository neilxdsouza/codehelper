#ifndef ABSTRACT_DATABASE_CODEGENERATOR_H
#define ABSTRACT_DATABASE_CODEGENERATOR_H

struct AbstractDataBaseCodeGenerator
{
	AbstractDataBaseCodeGenerator()
	{ }
	virtual void GenerateCode()=0;

	//virtual void SetOutputDirectory();
	//virtual void SetTableInfo();
protected:
	~AbstractDataBaseCodeGenerator()
	{ }


};

#endif /* ABSTRACT_DATABASE_CODEGENERATOR_H */
