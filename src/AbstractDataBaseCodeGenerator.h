#ifndef ABSTRACT_DATABASE_CODEGENERATOR_H
#define ABSTRACT_DATABASE_CODEGENERATOR_H

#include "std_headers.h"
#include "tree.h"

struct AbstractDataBaseCodeGenerator
{
public:
	TableInfoType * tableInfo_;
	std::string outputDirPrefix_;
	AbstractDataBaseCodeGenerator(TableInfoType * p_tabInfo,
				std::string & p_output_dir_path)

	: tableInfo_(p_tabInfo),
	  outputDirPrefix_(p_output_dir_path)			
	{ }
	virtual void GenerateCode()=0;

	virtual void print_sp_select_params(std::stringstream & p_sp_select_fields,
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname)=0;

	//virtual void SetOutputDirectory();
	//virtual void SetTableInfo();
protected:
	~AbstractDataBaseCodeGenerator()
	{ }
private:
	AbstractDataBaseCodeGenerator(const AbstractDataBaseCodeGenerator&);
	AbstractDataBaseCodeGenerator& operator=(const AbstractDataBaseCodeGenerator&);


};

#endif /* ABSTRACT_DATABASE_CODEGENERATOR_H */
