#ifndef ABSTRACT_CODE_GENERATOR_H
#define ABSTRACT_CODE_GENERATOR_H

#include "tree.h"
#include "AbstractDataBaseCodeGenerator.h"

struct AbstractCodeGenerator
{
	AbstractCodeGenerator(TableInfoType * p_TableInfoType_ptr,
			      AbstractDataBaseCodeGenerator * p_dbCodeGenerator,
			      std::string & p_outputDirPrefix)
		: tableInfo_(p_TableInfoType_ptr),
		dbCodeGenerator_(p_dbCodeGenerator),
		outputDirPrefix_(p_outputDirPrefix)
	{ }
	TableInfoType * tableInfo_;
	AbstractDataBaseCodeGenerator * dbCodeGenerator_;
	std::string outputDirPrefix_;
	//virtual void print_sql_provider(FILE *fptr)=0;
	//virtual void print_abstract_provider(FILE * fptr)=0;
	//virtual void print_sp(FILE * fptr)=0;
	//virtual void print_bll(FILE * fptr)=0;
	//virtual void print_ui(FILE * fptr)=0;
	virtual void GenerateCode(FILE * fptr)=0;

 
	protected:
	~AbstractCodeGenerator()
	{}
private:
	AbstractCodeGenerator(const AbstractCodeGenerator&);
	AbstractCodeGenerator& operator=(const AbstractCodeGenerator&);
};

#endif /* ABSTRACT_CODE_GENERATOR_H */
