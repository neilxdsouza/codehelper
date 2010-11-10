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
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level)=0;

	virtual void print_cpp_select_params(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level)=0;


	virtual void print_cpp_convert_db_fields_to_cpp2(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname)=0;


	virtual void print_sp_return_table_fields(std::stringstream & p_sp_select_fields_with_type)=0;
	virtual void print_sp_return_table_fields2(
			std::stringstream & p_sp_select_fields_with_type,
			bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level)=0;

	virtual void print_reader(bool with_pkey,
			bool rename_vars, std::string inner_join_tabname,
			std::vector< boost::shared_ptr<std::stringstream> > & p_vec_reader_str,
			int recursion_level, std::stringstream & dummy_defns, 
			bool descend)=0;

	virtual void print_sp_select_inner_joins2(stringstream & p_inner_join_str,
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level)=0;

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
