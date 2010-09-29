#include "std_headers.h"
#include "error.h"
#include "utils.h"

#include "PostgreSQLCodeGeneratorFactory.h"
#include "PostgreSQLCodeGenerator.h"

#include "std_using.h"

using boost::format;
using boost::io::group;

PostgreSQLCodeGenerator::PostgreSQLCodeGenerator(TableInfoType * p_tabInfo,
						 std::string & p_output_dir_path)
	: tableInfo_(p_tabInfo),
	  outputDirPrefix_(p_output_dir_path)			
{ }


void PostgreSQLCodeGenerator::GenerateCode()
{
	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;

	GenerateStoredProcedures();
	GenerateCppFuncs();
	
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
	insert_sp << boost::format("create or replace function sp_%1%_insert_%1%(")
		% tableInfo_->tableName_;
	print_sp_param_decls(insert_sp, INSERT);
	insert_sp << ") RETURNS INT AS $$ \n";
	insert_sp << "BEGIN\n";
	insert_sp << "\tINSERT INTO " << tableInfo_->tableName_ 
		<< "(";
	print_sp_fields( insert_sp, INSERT);
	insert_sp << ")";
	insert_sp << " values (";
	print_sp_params( insert_sp, INSERT);
	insert_sp << ") RETURNING ";
	print_sp_1st_param( insert_sp, INSERT);
	insert_sp << ";" << endl << "END\n$$ LANGUAGE plpgsql;\n";
	if(insert_sp)
		insert_sp.close();
			
			
}

void PostgreSQLCodeGenerator::print_sp_param_decls(ofstream & ofile, print_sp_params_mode mode)
{
	struct var_list * v_ptr=tableInfo_->param_list;
	if(mode==INSERT){
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		//fprintf(fptr, "\t@%s ", v_ptr->var_name.c_str());
		ofile << boost::format("\tp_%1% ") % v_ptr->var_name;
		print_sp_types(ofile,  v_ptr->var_type);
		if(v_ptr->var_type==NVARCHAR_TYPE||v_ptr->var_type==VARCHAR_TYPE
			|| v_ptr->var_type==NCHAR_TYPE	){
			//fprintf(fptr, "(%d)", v_ptr->arr_len);
			ofile << "(" << v_ptr->arr_len << ")";
		}
		v_ptr=v_ptr->prev;
		if(v_ptr){
			//fprintf(fptr, ",\n");
			ofile << ",\n";
		} else {
			//fprintf(fptr, "\n");
			ofile << "\n";
		}
	}
}

void PostgreSQLCodeGenerator::print_sp_params(ofstream & ofile, print_sp_params_mode mode)
{
	struct var_list * v_ptr=tableInfo_->param_list;
	if(mode==INSERT){
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		//fprintf(fptr, "\t@%s ", v_ptr->var_name.c_str());
		ofile << boost::format("\tp_%1% ") % v_ptr->var_name;
		v_ptr=v_ptr->prev;
		if(v_ptr){
			//fprintf(fptr, ",\n");
			ofile << ",\n";
		} else {
			//fprintf(fptr, "\n");
			ofile << "\n";
		}
	}
}


void PostgreSQLCodeGenerator::print_sp_fields(ofstream & ofile, print_sp_params_mode mode)
{
	struct var_list * v_ptr=tableInfo_->param_list;
	if(mode==INSERT){
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		//fprintf(fptr, "\t@%s ", v_ptr->var_name.c_str());
		ofile << boost::format("\t%1% ") % v_ptr->var_name;

		v_ptr=v_ptr->prev;
		if(v_ptr){
			//fprintf(fptr, ",\n");
			ofile << ",\n";
		} else {
			//fprintf(fptr, "\n");
			ofile << "\n";
		}
	}
}


void PostgreSQLCodeGenerator::print_sp_1st_param(ofstream & ofile, print_sp_params_mode mode)
{
	struct var_list * v_ptr=tableInfo_->param_list;
	ofile << boost::format("\tp_%1%") %v_ptr->var_name;
}

void PostgreSQLCodeGenerator::GenerateCppFuncs()
{
	string cpp_db_impl_fname(string(outputDirPrefix_.c_str()
				       +string("/PSqlDbImplementation.cpp"))); 
	std::ofstream cpp_db_impl(cpp_db_impl_fname.c_str(), ios_base::out|ios_base::trunc);

	if(!cpp_db_impl){
		string err_msg="unable to open " + cpp_db_impl_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}

	print_cpp_db_impl_header(cpp_db_impl);
	PrintGetConn(cpp_db_impl);
	PrintCppInsertFunc(cpp_db_impl);

}


void PostgreSQLCodeGenerator::print_lower_fname(ofstream & file)
{
	char buffer[MAX_VAR_LEN];
	if(tableInfo_->tableName_.length() < MAX_VAR_LEN-1){
		strcpy(buffer, tableInfo_->tableName_.c_str());
		buffer[0]=tolower(buffer[0]);
		string lower_name(buffer);
		//fprintf(fptr, "%s", buffer);
		file << lower_name;
	}
}

void PostgreSQLCodeGenerator::print_cpp_db_impl_header(ofstream & cpp_db_impl)
{
	cpp_db_impl << boost::format("#include <postgresql/libpq-fe.h>\n");
	cpp_db_impl << boost::format("#include \"%1%_bll.h\">\n") % tableInfo_->tableName_;
}

void PostgreSQLCodeGenerator::PrintCppInsertFunc(ofstream & cpp_db_impl)
{
	cpp_db_impl << boost::format("int Insert%1%(Biz%1% & ") % tableInfo_->tableName_;
	print_lower_fname(cpp_db_impl);
	cpp_db_impl << ")\n{\n";

	cpp_db_impl << "\tPGconn      *conn=GetPGConn();\n";
	cpp_db_impl << "\n}\n";
}

void PostgreSQLCodeGenerator::PrintGetConn(ofstream & cpp_db_impl)
{
	cpp_db_impl << "\nPGconn * GetPGConn()\n{\n";

	cpp_db_impl << "\tconst char  *conninfo;\n";
	cpp_db_impl << "\tPGconn      *conn;\n";
	cpp_db_impl << "\tconninfo = \"dbname = nxd\";\n";

	cpp_db_impl << "\tconn = PQconnectdb(conninfo);\n";
	cpp_db_impl << "\tif (PQstatus(conn) != CONNECTION_OK)\n";
	cpp_db_impl << "\t{\n";
	cpp_db_impl << "\t	fprintf(stderr, \"Connection to database failed: %s\",\n";
	cpp_db_impl << "\t\t	PQerrorMessage(conn));\n";
	cpp_db_impl << "\t	exit_nicely(conn);\n";
	cpp_db_impl << "\t}\n";
	cpp_db_impl << "\treturn conn;\n";

	cpp_db_impl << "\n}\n";
}
