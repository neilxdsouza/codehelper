#ifndef POSTGRESQL_CODEGENERATOR_H
#define POSTGRESQL_CODEGENERATOR_H

#include "std_headers.h"
#include "tree.h"

struct PostgreSQLCodeGenerator: public AbstractDataBaseCodeGenerator
{
public:
	//TableInfoType * tableInfo_;
	//std::string outputDirPrefix_;

public:
	PostgreSQLCodeGenerator(TableInfoType * p_tabInfo,
				std::string & p_output_dir_path);
	virtual void GenerateCode();
	virtual void GenerateStoredProcedures();
	//void print_sp_param_decls(ofstream & ofile, print_sp_params_mode mode);
	string print_sp_param_decls(print_sp_params_mode mode);
	string print_sp_params( print_sp_params_mode mode);
	string print_sp_fields( print_sp_params_mode mode);
	void print_sp_1st_param(ofstream & ofile, print_sp_params_mode mode);
	string print_sp_pkey_param();
	string print_sp_pkey_field();
	void print_lower_fname(ofstream & file);
	string print_lower_table_name();
	void print_cpp_db_impl_header(ofstream & cpp_db_impl);
	void GenerateDB_h();
public:
	virtual void GenerateInsertSP();
	virtual void GenerateSelectSP();
	virtual void GenerateCreateSQL();
	void GenerateCppFuncs();
	void PrintCppInsertFunc(ofstream & ofile);
	string PrintCppSelectFunc();
	void PrintGetConn(ofstream & cpp_db_impl);
	void PrintConnCloser(ofstream & cpp_db_impl);
	void PrintMallocDeleter(ofstream & cpp_db_impl);
	void print_exit_nicely(ofstream & cpp_db_impl);
	std::string print_sp_search_key_params();
	std::string print_cpp_search_key_params();
	std::string print_sp_search_key_fields();
	std::string print_sp_search_key_whereclause();

	void print_sp_select_fields(std::stringstream & p_sp_select_fields,
			std::stringstream & p_sp_select_fields_with_type);
	void print_sp_select_params(std::stringstream & p_sp_select_fields, 
			std::stringstream & p_sp_select_fields_with_type,
			bool with_pkey, bool rename_vars, string inner_join_tabname);

	void print_cpp_select_field_positions(std::stringstream & p_sp_select_fields,
					std::stringstream & p_sp_select_fields_with_type);
	void print_cpp_select_params(std::stringstream & p_sp_select_fields,
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname);
	void print_cpp_convert_db_fields_to_cpp(std::stringstream& convert_fields_str);
	void print_cpp_convert_db_fields_to_cpp2(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname);
private:
	PostgreSQLCodeGenerator(const PostgreSQLCodeGenerator &);
	PostgreSQLCodeGenerator& operator= (const PostgreSQLCodeGenerator &);
protected:
	~PostgreSQLCodeGenerator()
	{ }
};

#endif /* POSTGRESQL_CODEGENERATOR_H */
