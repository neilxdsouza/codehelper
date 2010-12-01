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
	virtual void GenerateSelectSingleRecordSP();
	void GenerateCppFuncs();
	void PrintCppInsertFunc(ofstream & ofile);
	string PrintCppSelectFunc();
	string PrintCppSelectSingleFunc();
	void PrintGetConn(ofstream & cpp_db_impl);
	void PrintConnCloser(ofstream & cpp_db_impl);
	void PrintMallocDeleter(ofstream & cpp_db_impl);
	void print_exit_nicely(ofstream & cpp_db_impl);
	std::string print_sp_search_key_params();
	std::string print_cpp_search_key_params();
	std::string print_sp_search_key_fields();
	std::string print_sp_search_key_whereclause();

	void print_sp_select_fields(std::stringstream & p_sp_select_fields);
	void print_sp_select_params(std::stringstream & p_sp_select_fields, 
			bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level);

	void print_cpp_select_field_positions(
					std::stringstream & p_sp_select_fields_with_type);
	void print_cpp_select_params(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level);
	void print_cpp_convert_db_fields_to_cpp(std::stringstream& convert_fields_str);
	void print_cpp_convert_db_fields_to_cpp2(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname);
	std::string PrintGetSingleRecord_h();
	std::string PrintGetSingleRecord();
	std::string print_reader_param_with_cast(var_list* v_ptr, std::string ref_table_name);
	void print_reader(bool with_pkey, bool rename_vars, std::string inner_join_tabname, 
		std::vector<boost::shared_ptr<std::stringstream> > & p_vec_reader_str, int recursion_level,
		std::stringstream & dummy_defns,
		bool descend);
	std::string GenerateRandomData();

	void print_sp_return_table_fields(std::stringstream & p_sp_select_fields_with_type);
	void print_sp_return_table_fields2(
			std::stringstream & p_sp_select_fields_with_type,
			bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level);

	std::string print_sp_select_inner_joins();
	void print_sp_select_inner_joins2(stringstream & p_inner_join_str,
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level);

	std::string print_sp_session_params();
	std::string print_cpp_session_params();
	std::string print_cpp_sp_invoc_search_keys(int & nActualParams);
	std::string print_cpp_sp_invoc_session_keys(int & nActualParams);
	void GenerateAuthenticateLoginSP();
	void GenerateCppAuthenticateLogin(std::stringstream & p_func_decl,
			std::stringstream& p_func_defn);
private:
	PostgreSQLCodeGenerator(const PostgreSQLCodeGenerator &);
	PostgreSQLCodeGenerator& operator= (const PostgreSQLCodeGenerator &);
protected:
	~PostgreSQLCodeGenerator()
	{ }
};

#endif /* POSTGRESQL_CODEGENERATOR_H */
