#ifndef POSTGRESQL_CODEGENERATOR_H
#define POSTGRESQL_CODEGENERATOR_H

#include "std_headers.h"
#include "tree.h"

struct PostgreSQLCodeGenerator: public AbstractDataBaseCodeGenerator
{
public:
	TableInfoType * tabInfo_;
	std::string outputDirPrefix_;

public:
	PostgreSQLCodeGenerator(TableInfoType * p_tabInfo,
				std::string & p_output_dir_path);
	virtual void GenerateCode();
	virtual void GenerateStoredProcedures();
	//virtual void SetOutputDirectory(std::string & output_code_directory_prefix);
public:
	virtual void GenerateInsertSP();
};

#endif /* POSTGRESQL_CODEGENERATOR_H */
