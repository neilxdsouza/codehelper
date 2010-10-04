#ifndef POSTGRESQL_CODEGENERATOR_H
#define POSTGRESQL_CODEGENERATOR_H

#include "std_headers.h"
#include "tree.h"

struct PostgreSQLCodeGenerator: public AbstractDataBaseCodeGenerator
{
public:
	TableInfoType * tableInfo_;
	std::string outputDirPrefix_;

public:
	PostgreSQLCodeGenerator(TableInfoType * p_tabInfo,
				std::string & p_output_dir_path);
	virtual void GenerateCode();
	virtual void GenerateStoredProcedures();
	void print_sp_param_decls(ofstream & ofile, print_sp_params_mode mode);
	void print_sp_params(ofstream & ofile, print_sp_params_mode mode);
	void print_sp_fields(ofstream & ofile, print_sp_params_mode mode);
	void print_sp_1st_param(ofstream & ofile, print_sp_params_mode mode);
	string print_sp_pkey_param();
	string print_sp_pkey_field();
	void print_lower_fname(ofstream & file);
	string print_lower_table_name();
	void print_cpp_db_impl_header(ofstream & cpp_db_impl);
	void GenerateDB_h();
public:
	virtual void GenerateInsertSP();
	void GenerateCppFuncs();
	void PrintCppInsertFunc(ofstream & ofile);
	void PrintGetConn(ofstream & cpp_db_impl);
	void PrintConnCloser(ofstream & cpp_db_impl);
	void PrintMallocDeleter(ofstream & cpp_db_impl);
	void print_exit_nicely(ofstream & cpp_db_impl);
private:
	PostgreSQLCodeGenerator(const PostgreSQLCodeGenerator &);
	PostgreSQLCodeGenerator& operator= (const PostgreSQLCodeGenerator &);
};

#endif /* POSTGRESQL_CODEGENERATOR_H */
