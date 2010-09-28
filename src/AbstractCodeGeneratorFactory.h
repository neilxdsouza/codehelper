#ifndef ABSTRACT_CODE_GENERATOR_FACTORY_H
#define ABSTRACT_CODE_GENERATOR_FACTORY_H

#include "AbstractCodeGenerator.h"
#include "AbstractDataBaseCodeGeneratorFactory.h"
#include "tree.h"

struct AbstractCodeGeneratorFactory
{
	AbstractDataBaseCodeGeneratorFactory * dbCodeGeneratorFactory_;
	std::string outputCodeDirectoryPrefix_;
	AbstractCodeGeneratorFactory(AbstractDataBaseCodeGeneratorFactory *
				     p_dbCodeGeneratorFactory, std::string &
				     p_outputCodeDirectoryPrefix)
	: dbCodeGeneratorFactory_(p_dbCodeGeneratorFactory),
		outputCodeDirectoryPrefix_(p_outputCodeDirectoryPrefix)
	{ }
	//virtual void print_sql_provider(FILE *fptr)=0;
	//virtual void print_abstract_provider(FILE * fptr)=0;
	//virtual void print_sp(FILE * fptr)=0;
	//virtual void print_bll(FILE * fptr)=0;
	//virtual void print_ui(FILE * fptr)=0;
	//virtual void print(FILE * fptr)=0;
	virtual AbstractCodeGenerator*
	CreateCodeGenerator(TableInfoType * p_TableInfoType_ptr)=0;
	
	protected:
	~AbstractCodeGeneratorFactory()
	{ }
private:
	AbstractCodeGeneratorFactory(const AbstractCodeGeneratorFactory&  );
	AbstractCodeGeneratorFactory& operator=(const AbstractCodeGeneratorFactory&);
};

#endif /* ABSTRACT_CODE_GENERATOR_FACTORY_H */
