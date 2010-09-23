#ifndef ABSTRACT_CODE_GENERATOR_H
#define ABSTRACT_CODE_GENERATOR_H

struct AbstractCodeGenerator
{
	AbstractCodeGenerator(TableInfoType * p_TableInfoType_ptr)
		: tableInfo_(p_TableInfoType_ptr)
	{ }
	TableInfoType * tableInfo_;
	virtual void print_sql_provider(FILE *fptr)=0;
	virtual void print_abstract_provider(FILE * fptr)=0;
	virtual void print_sp(FILE * fptr)=0;
	virtual void print_bll(FILE * fptr)=0;
	virtual void print_ui(FILE * fptr)=0;
	virtual void print(FILE * fptr)=0;

	protected:
	~AbstractCodeGenerator();
};

#endif /* ABSTRACT_CODE_GENERATOR_H */
