#include "std_headers.h"
#include "error.h"
#include "utils.h"

#include "PostgreSQLCodeGeneratorFactory.h"
#include "PostgreSQLCodeGenerator.h"

#include "std_using.h"

using boost::format;
using boost::io::group;
extern char project_namespace[];

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
	GenerateDB_h();
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
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_insert_postgres.sql"))); 
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
		if(v_ptr->var_type == COMPOSITE_TYPE) {
			v_ptr=v_ptr->prev;
			continue;
		}
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
					+ tableInfo_->tableName_ 
					+ string("_db_postgres.cpp"))); 
	std::ofstream cpp_db_impl(cpp_db_impl_fname.c_str(), ios_base::out|ios_base::trunc);

	if(!cpp_db_impl){
		string err_msg="unable to open " + cpp_db_impl_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	print_cpp_db_impl_header(cpp_db_impl);
	PrintConnCloser(cpp_db_impl);
	print_exit_nicely(cpp_db_impl);
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

string PostgreSQLCodeGenerator::print_lower_table_name()
{
	char buffer[MAX_VAR_LEN];
	if(tableInfo_->tableName_.length() < MAX_VAR_LEN-1){
		strcpy(buffer, tableInfo_->tableName_.c_str());
		buffer[0]=tolower(buffer[0]);
		string lower_name(buffer);
		//fprintf(fptr, "%s", buffer);
		return lower_name;
	}
	return string("");
}

void PostgreSQLCodeGenerator::print_cpp_db_impl_header(ofstream & cpp_db_impl)
{
	cpp_db_impl << boost::format("#include <postgresql/libpq-fe.h>\n");
	cpp_db_impl << boost::format("#include <boost/shared_ptr.hpp>\n");
	cpp_db_impl << boost::format("#include <boost/scoped_ptr.hpp>\n");
	cpp_db_impl << boost::format("#include <boost/date_time/gregorian/gregorian.hpp>\n");
	cpp_db_impl << boost::format("#include <cstdio>\n");
	cpp_db_impl << boost::format("#include <sstream>\n");
	cpp_db_impl << boost::format("#include \"%1%_bll.h\"\n") % tableInfo_->tableName_;
}

void PostgreSQLCodeGenerator::PrintCppInsertFunc(ofstream & cpp_db_impl)
{
	cpp_db_impl << boost::format("int Insert%1%(Biz%1% & %2%)\n{") % tableInfo_->tableName_
		% print_lower_table_name();
	//print_lower_fname(cpp_db_impl);
	//cpp_db_impl << ")\n{\n";
	cpp_db_impl << "\tboost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());\n";

	//const char *paramValues[2];
	//int			paramLengths[2];
	//int			paramFormats[2];
	cpp_db_impl << boost::format("\tconst char * paramValues[%1%];\n"
			"\tstd::stringstream ss_param_values[%1%];\n") % 
		tableInfo_->vec_var_list.size();

	for (int i=0; i<tableInfo_->vec_var_list.size(); ++i) {
		if(tableInfo_->vec_var_list[i]->var_type==COMPOSITE_TYPE)
			continue;
		if(tableInfo_->vec_var_list[i]->var_type==DATETIME_TYPE){
			cpp_db_impl << format("\tss_param_values[%1%] << to_simple_string(%2%.Get_%3%());\n")
				% i % print_lower_table_name()
				% tableInfo_->vec_var_list[i]->var_name;
		} else {
			cpp_db_impl << format("\tss_param_values[%1%] << %2%.Get_%3%();\n")
				% i % print_lower_table_name()
				% tableInfo_->vec_var_list[i]->var_name;
		}
		cpp_db_impl << boost::format("\tparamValues[%1%]=ss_param_values[%1%].str().c_str();\n")
				% i;
	}

	cpp_db_impl << boost::format("\tPGresult *res=PQexecParams(conn.get(), \n\t\t\"select * from sp_%1%_insert_%1%(\"\n") %
		tableInfo_->tableName_;

	bool print_comma=true;
	cpp_db_impl << "\t\t\t\"";
	for (int i=0; i<tableInfo_->vec_var_list.size(); ++i) {
		if (i==tableInfo_->vec_var_list.size()-1){
			print_comma=false;
		}
		cpp_db_impl << format("$%1%") % (i+1);
		if (print_comma) {
			cpp_db_impl << ",";
		}
	}
	cpp_db_impl << boost::format("\", %1%, NULL, paramValues, NULL, NULL,0);\n") %
			tableInfo_->vec_var_list.size();
	
	cpp_db_impl << "\tif (PQresultStatus(res) != PGRES_TUPLES_OK){\n";
	cpp_db_impl << "\t\tint res_status = PQresultStatus(res);\n";
	cpp_db_impl << "\t\tprintf(\"res_status=%d, PGRES_COMMAND_OK = %d, PGRES_TUPLES_OK=%d\",\n"
				 << "\t\t\tres_status, PGRES_COMMAND_OK, PGRES_TUPLES_OK);\n";

	cpp_db_impl << "\t\tfprintf(stderr, \"insert employee failed: %%s\", PQerrorMessage(conn.get()));\n";
	cpp_db_impl << "\t\tPQclear(res);\n";
	fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, 
		string("Fix me : exit_nicely may not be requires since the PGconn has a custom deleter which closes the connection\n"));
	cpp_db_impl << "\t\texit_nicely(conn.get());\n";
	cpp_db_impl << "\t}\n";


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

void PostgreSQLCodeGenerator::PrintConnCloser(ofstream & cpp_db_impl)
{
	cpp_db_impl << "class ConnCloser\n"
		<< "{\n"
		<< "public:\n"
		<< "\tvoid operator() (PGconn * conn)\n"
		<< "\t{\n"
		<< "\t\tPQfinish(conn);\n"
		<< "\t}\n"
		<< "};\n";
}

void PostgreSQLCodeGenerator::print_exit_nicely(ofstream & cpp_db_impl)
{
	cpp_db_impl << "static void\n";
	cpp_db_impl << "exit_nicely(PGconn *conn)\n";
	cpp_db_impl << "{\n";
	cpp_db_impl << "\tPQfinish(conn);\n";
	cpp_db_impl << "\texit(1);\n";
	cpp_db_impl << "}\n";
}

void PostgreSQLCodeGenerator::GenerateDB_h()
{
	string db_h_fname(string(outputDirPrefix_.c_str()
					+ tableInfo_->tableName_ 
					+ string("_db_postgres.h"))); 
	std::ofstream db_h(db_h_fname.c_str(), ios_base::out|ios_base::trunc);

	db_h << boost::format("#include \"%1%_bll.h\"\n")
			% tableInfo_->tableName_;
	
	db_h << boost::format("namespace %1% { namespace db { namespace %2% {\n")
			% project_namespace % tableInfo_->tableName_;

	db_h << boost::format("int Insert%1%(Biz%1% & ") % tableInfo_->tableName_;
	print_lower_fname(db_h);
	db_h << ");\n";

	db_h << boost::format("} /* close namespace %1% */ } /*close namespace db*/ } /* close namespace %2% */\n")
			% tableInfo_->tableName_
			% project_namespace ;

}
