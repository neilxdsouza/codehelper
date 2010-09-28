#include "std_headers.h"
#include "error.h"

#include "PostgreSQLCodeGeneratorFactory.h"
#include "PostgreSQLCodeGenerator.h"

#include "std_using.h"

using boost::format;
using boost::io::group;

PostgreSQLCodeGenerator::PostgreSQLCodeGenerator(TableInfoType * p_tabInfo,
						 std::string & p_output_dir_path)
	: tabInfo_(p_tabInfo),
	  outputDirPrefix_(p_output_dir_path)			
{ }


void PostgreSQLCodeGenerator::GenerateCode()
{
	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;

	GenerateStoredProcedures();
	
	cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
}


void PostgreSQLCodeGenerator::GenerateStoredProcedures()
{
	GenerateInsertSP();
}

/*
void PostgreSQLCodeGenerator::SetOutputDirectory(output_code_directory_prefix)
{
	outputDirPrefix_=output_code_directory_prefix;
}
*/

void PostgreSQLCodeGenerator::GenerateInsertSP()
{
	cout << "outputDirPrefix_: " << outputDirPrefix_ << endl;
	string sp_insert_fname (string(outputDirPrefix_.c_str()
				       +string("/sp_insert_postgres.sql"))); 
	std::ofstream insert_sp(sp_insert_fname.c_str(), ios_base::out|ios_base::trunc);
	if(!insert_sp){
		string err_msg="unable to open " + sp_insert_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	insert_sp << "create or replace function sp_insert";
	//insert_sp.close();
			
			
}
