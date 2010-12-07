#include "std_headers.h"
#include "error.h"
#include "utils.h"
#include "CppCodeGenerator.h"
#include "global_options.h"

#include "PostgreSQLCodeGeneratorFactory.h"
#include "PostgreSQLCodeGenerator.h"
#include "TableCollectionSingleton.hpp"

#include "std_using.h"
#include "global_options.h"

using boost::format;
using boost::io::group;
//extern char project_namespace[];
using global_options::project_namespace;

PostgreSQLCodeGenerator::PostgreSQLCodeGenerator(TableInfoType * p_tabInfo,
						 std::string & p_output_dir_path)
	//: tableInfo_(p_tabInfo),
	//  outputDirPrefix_(p_output_dir_path)			
	:AbstractDataBaseCodeGenerator(p_tabInfo, p_output_dir_path)
{ }


void PostgreSQLCodeGenerator::GenerateCode()
{
//	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
//		% __PRETTY_FUNCTION__;
	GenerateStoredProcedures();
	GenerateCppFuncs();
	GenerateDB_h();
//	cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
//		% __PRETTY_FUNCTION__;
}


void PostgreSQLCodeGenerator::GenerateStoredProcedures()
{
	GenerateInsertSP();
	GenerateSelectSP();
	GenerateSelectSingleRecordSP();
	GenerateCreateSQL();
	GenerateRandomData();
	if (tableInfo_->tab_options.is_login_page) {
		GenerateAuthenticateLoginSP();
	}
}

/*
void PostgreSQLCodeGenerator::SetOutputDirectory(output_code_directory_prefix)
{
	outputDirPrefix_=output_code_directory_prefix;
}
*/

void PostgreSQLCodeGenerator::GenerateInsertSP()
{
	stringstream insert_sp_decl_str, insert_sp_body_str;

	insert_sp_decl_str << boost::format("function sp_%1%_insert_%1%(")
		% tableInfo_->tableName_;
	insert_sp_decl_str << print_sp_param_decls( INSERT);
	insert_sp_decl_str << ")";
	insert_sp_body_str << "AS $$\n";
	insert_sp_body_str << "BEGIN\n";
	insert_sp_body_str << "\tINSERT INTO " << tableInfo_->tableName_ 
		<< "(";
	insert_sp_body_str << print_sp_fields(  INSERT);
	insert_sp_body_str << "\t\t) values (";
	insert_sp_body_str << print_sp_params(  INSERT);
	insert_sp_body_str << "\t\t);\n";
	insert_sp_body_str << "\tselect last_value into ";
	//print_sp_1st_param( insert_sp_body_str, INSERT);
	insert_sp_body_str << "p_" << tableInfo_->param_list->var_name;
	
	insert_sp_body_str << boost::format(" from %1%_%2%_seq;\n")
		% tableInfo_->tableName_ % print_sp_pkey_field();
	insert_sp_body_str << endl << "END\n$$ LANGUAGE plpgsql;\n";

	// cout << "outputDirPrefix_: " << outputDirPrefix_ << endl;
	string sp_insert_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_insert_postgres.sql"))); 
	std::ofstream insert_sp(sp_insert_fname.c_str(), ios_base::out|ios_base::trunc);
	if(!insert_sp){
		string err_msg="unable to open " + sp_insert_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	insert_sp << "create or replace "
		<< insert_sp_decl_str.str() << endl
		<< insert_sp_body_str.str();

	string sp_drop_insert_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_drop_insert_postgres.sql"))); 
	std::ofstream drop_insert_sp(sp_drop_insert_fname.c_str(), ios_base::out|ios_base::trunc);
	if(!drop_insert_sp){
		string err_msg="unable to open " + sp_drop_insert_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	drop_insert_sp << "drop "
		<< insert_sp_decl_str.str() << ";"
		<< endl;

}

string PostgreSQLCodeGenerator::print_sp_param_decls(print_sp_params_mode mode)
{
	struct var_list * v_ptr=tableInfo_->param_list;
	if(mode==INSERT){
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
		v_ptr=v_ptr->prev;
	}
	stringstream sp_param_decls_str;
	while(v_ptr){
		//fprintf(fptr, "\t@%s ", v_ptr->var_name.c_str());
		if(v_ptr->var_type == COMPOSITE_TYPE) {
			v_ptr=v_ptr->prev;
			continue;
		} /*else if (v_ptr->options.session) {
			v_ptr=v_ptr->prev;
			continue;
		}
		*/
		sp_param_decls_str << boost::format("\tp_%1% %2%") % v_ptr->var_name
			% print_sp_types(v_ptr->var_type) ;
		if(v_ptr->var_type==NVARCHAR_TYPE||v_ptr->var_type==VARCHAR_TYPE
			|| v_ptr->var_type==NCHAR_TYPE	){
			//fprintf(fptr, "(%d)", v_ptr->arr_len);
			sp_param_decls_str << "(" << v_ptr->arr_len << ")";
		}
		v_ptr=v_ptr->prev;
		if (v_ptr==NULL && mode==INSERT){
			sp_param_decls_str << ",\n";
		} else {
			if (v_ptr){
				sp_param_decls_str << ",\n";
			} else {
				sp_param_decls_str << "\n";
			}
		}
	}
	if (mode == INSERT) {
		sp_param_decls_str	<< "\tout " 
			<< print_sp_pkey_param() << " int\n";
	}
	return sp_param_decls_str.str();
}

string PostgreSQLCodeGenerator::print_sp_params( print_sp_params_mode mode)
{
	stringstream sp_params;
	struct var_list * v_ptr=tableInfo_->param_list;
	string tab_indent("\t\t\t");
	sp_params << endl;
	if(mode==INSERT){
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
		sp_params << tab_indent << "DEFAULT,\n";
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		if(v_ptr->var_type == COMPOSITE_TYPE) {
			v_ptr=v_ptr->prev;
			continue;
		}
		sp_params << tab_indent << boost::format("p_%1%") % v_ptr->var_name;
		v_ptr=v_ptr->prev;
		if(v_ptr){
			sp_params << ",\n";
		} else {
			sp_params << "\n";
		}
	}
	return sp_params.str();
}


string PostgreSQLCodeGenerator::print_sp_fields( print_sp_params_mode mode)
{
	stringstream sp_fields;
	struct var_list * v_ptr=tableInfo_->param_list;
	string tab_indent("\t\t\t");
	sp_fields << endl;
	//if(mode==INSERT){
	//// Skip the 1st param - assume that it is the ouput parameter and print it out last
	//	v_ptr=v_ptr->prev;
	//}
	while(v_ptr){
		if(v_ptr->var_type == COMPOSITE_TYPE) {
			v_ptr=v_ptr->prev;
			continue;
		}
		sp_fields << tab_indent << boost::format("%1%") % v_ptr->var_name;
		v_ptr=v_ptr->prev;
		if (mode==INSERT) {
			if(v_ptr){
				sp_fields << ",\n";
			} else {
				sp_fields << "\n";
			}
		} else {
			// because of the Window function rank() ... over order by ...
			// we need the extra comma
			sp_fields << ",\n";
		}

	}
	return sp_fields.str();
}


/*
void PostgreSQLCodeGenerator::print_sp_1st_param(ofstream & ofile, print_sp_params_mode mode)
{
	struct var_list * v_ptr=tableInfo_->param_list;
	ofile << boost::format("\tp_%1%") %v_ptr->var_name;
}
*/

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
	PrintMallocDeleter(cpp_db_impl);
	print_exit_nicely(cpp_db_impl);
	PrintGetConn(cpp_db_impl);
	PrintCppInsertFunc(cpp_db_impl);

	if (tableInfo_->tab_options.is_login_page) {
		stringstream cpp_auth_func_decl, cpp_auth_func_defn;
		GenerateCppAuthenticateLogin(cpp_auth_func_decl, cpp_auth_func_defn);
		cpp_db_impl << cpp_auth_func_defn.str();
		AddFuncDecl(cpp_auth_func_decl.str() );
	}
	cpp_db_impl << PrintCppSelectFunc();
	cpp_db_impl << PrintCppSelectSingleFunc();
	cpp_db_impl << PrintGetSingleRecord();
	cpp_db_impl << boost::format("} /* close namespace %1% */ } /*close namespace db*/ } /* close namespace %2% */\n")
			% tableInfo_->tableName_
			% project_namespace ;
}


void PostgreSQLCodeGenerator::print_lower_fname(ofstream & file)
{
	char buffer[MAX_VAR_LEN];
	if(tableInfo_->tableName_.length() < MAX_VAR_LEN-1){
		strcpy(buffer, tableInfo_->tableName_.c_str());
		buffer[0]=tolower(buffer[0]);
		string lower_name(buffer);
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
	cpp_db_impl << boost::format("#include <boost/lexical_cast.hpp>\n");
	cpp_db_impl << boost::format("#include <cstdio>\n");
	cpp_db_impl << boost::format("#include <cstring>\n");
	cpp_db_impl << boost::format("#include <sstream>\n");
	cpp_db_impl << boost::format("#include \"%1%_bll.h\"\n") % tableInfo_->tableName_;
	cpp_db_impl << boost::format("#include \"%1%_db_postgres.h\"\n") % tableInfo_->tableName_;
	cpp_db_impl << boost::format("namespace %1% { namespace db { namespace %2% {\n")
			% project_namespace % tableInfo_->tableName_;
}

void PostgreSQLCodeGenerator::PrintCppInsertFunc(ofstream & cpp_db_impl)
{
	cpp_db_impl << boost::format("int Insert%1%(Biz%1% & %2%)\n{\n") % tableInfo_->tableName_
		% print_lower_table_name();
	cpp_db_impl << "\tboost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());\n";
	cpp_db_impl << boost::format("\tstd::vector<boost::shared_ptr<char> > char_ptr_vec(%1%);\n")
		% tableInfo_->vec_var_list.size() ;
	cpp_db_impl << boost::format("\tconst char * paramValues[%1%];\n"
			"\tstd::stringstream ss_param_values[%1%];\n") % 
		tableInfo_->vec_var_list.size();
	// skip 1st param - assume to be primary key
	int nActualParams=0;
	for (int i=1 ; i<tableInfo_->vec_var_list.size(); ++i) {
		if(tableInfo_->vec_var_list[i]->var_type==COMPOSITE_TYPE)
			continue;
		if(tableInfo_->vec_var_list[i]->var_type==DATETIME_TYPE){
			cpp_db_impl << format("\tss_param_values[%1%] << to_simple_string(%2%.Get_%3%());\n")
				% nActualParams % print_lower_table_name()
				% tableInfo_->vec_var_list[i]->var_name;
		} else {
			cpp_db_impl << format("\tss_param_values[%1%] << %2%.Get_%3%();\n")
				% nActualParams % print_lower_table_name()
				% tableInfo_->vec_var_list[i]->var_name;
		}
		cpp_db_impl << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
				% nActualParams;
		cpp_db_impl << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
		cpp_db_impl << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
				% nActualParams;
		++nActualParams;
	}
	cpp_db_impl << boost::format("\tPGresult *res=PQexecParams(conn.get(), \n\t\t\"select * from sp_%1%_insert_%1%(\"\n") %
		tableInfo_->tableName_;
	bool print_comma=true;
	cpp_db_impl << "\t\t\t\"";
	// skip 1st param - assume to be primary key
	for (int i=1, j=0; i<tableInfo_->vec_var_list.size(); ++i) {
		if(tableInfo_->vec_var_list[i]->var_type==COMPOSITE_TYPE)
			continue;
		if (i==tableInfo_->vec_var_list.size()-1){
			print_comma=false;
		}
		cpp_db_impl << format("$%1%::%2%") % (++j)
			% print_sp_types(tableInfo_->vec_var_list[i]->var_type);
		if (print_comma) {
			cpp_db_impl << ",";
		}
	}
	cpp_db_impl << boost::format(")\", %1%, NULL, paramValues, NULL, NULL,0);\n") %
			nActualParams;
	cpp_db_impl << "\tif (PQresultStatus(res) != PGRES_TUPLES_OK){\n";
	cpp_db_impl << "\t\tint res_status = PQresultStatus(res);\n";
	cpp_db_impl << "\t\tprintf(\"res_status=%d, PGRES_COMMAND_OK = %d, PGRES_TUPLES_OK=%d\\n\",\n"
				 << "\t\t\tres_status, PGRES_COMMAND_OK, PGRES_TUPLES_OK);\n";
	cpp_db_impl << "\t\tfprintf(stderr, \"insert employee failed: %s\", PQerrorMessage(conn.get()));\n";
	cpp_db_impl << "\t\tPQclear(res);\n";
	fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, 
		string("Fix me : exit_nicely may not be required since the PGconn has a custom deleter which closes the connection\n"));
	cpp_db_impl << "\t\texit_nicely(conn.get());\n";
	cpp_db_impl << "\t} else {\n";
	// cpp_db_impl << "\t	int nTuples = PQntuples(res);\n";
	// cpp_db_impl << "\t	int nFields = PQnfields(res);\n";
	// cpp_db_impl << "\t	printf( \"nTuples: %d, nFields=%d\\n\", nTuples, nFields);\n";
	// cpp_db_impl << "\t	for(int i=0; i<nFields; ++i){\n";
	// cpp_db_impl << "\t		char * fname=PQfname(res, i);\n";
	// cpp_db_impl << "\t		printf(\"fname: %s\\n\", fname);\n";
	// cpp_db_impl << "\t	}\n";
	// cpp_db_impl << "\t	char * value=PQgetvalue(res, 0, 0);\n";
	// cpp_db_impl << "\t	printf(\"value: %s\\n\", value);\n";
	cpp_db_impl << "\t\tint32_t r_" << tableInfo_->param_list->var_name  << "_fnum = PQfnumber(res, \"p_" 
			<< tableInfo_->param_list->var_name << "\");\n";
	cpp_db_impl << "\t\t" << tableInfo_->param_list->print_cpp_var_type() << " pkey = boost::lexical_cast< "
		<< tableInfo_->param_list->print_cpp_var_type() << " > (PQgetvalue (res, 0, r_" 
		<< tableInfo_->param_list->var_name << "_fnum));\n\t\treturn pkey;\n";

	cpp_db_impl << "\t}\n";
	cpp_db_impl << "\n}\n";
}

void PostgreSQLCodeGenerator::PrintGetConn(ofstream & cpp_db_impl)
{
	cpp_db_impl << "\nPGconn * GetPGConn()\n{\n";
	cpp_db_impl << "\tconst char  *conninfo;\n";
	cpp_db_impl << "\tPGconn      *conn;\n";
	cpp_db_impl << "\tconninfo = \"dbname=" << global_options::database_name 
		<< " port=" << global_options::database_port << "\";\n";
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


void PostgreSQLCodeGenerator::PrintMallocDeleter(ofstream & cpp_db_impl)
{
	cpp_db_impl << "class MallocDeleter\n"
		<< "{\n"
		<< "public:\n"
		<< "\tvoid operator() (char * ptr)\n"
		<< "\t{\n"
		<< "\t\tfree(ptr);\n"
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
	using boost::format;
	string db_h_fname(string(outputDirPrefix_.c_str()
					+ tableInfo_->tableName_ 
					+ string("_db_postgres.h"))); 
	std::ofstream db_h(db_h_fname.c_str(), ios_base::out|ios_base::trunc);
	db_h << "#include <postgresql/libpq-fe.h>\n";
	db_h << "#include <boost/shared_ptr.hpp>\n";
	db_h << format("#include \"%1%_bll.h\"\n")
			% tableInfo_->tableName_;
	db_h << format("namespace %1% { namespace db { namespace %2% {\n")
			% project_namespace % tableInfo_->tableName_;
	db_h << format("int Insert%1%(Biz%1% & ") % tableInfo_->tableName_;
	print_lower_fname(db_h);
	db_h << ");\n";
	/* Print Get Function signature using a different style */
	stringstream get_func_signature;
	get_func_signature << format("std::vector<boost::shared_ptr<Biz%1%> > Get%1%") %
		tableInfo_->tableName_;
	stringstream get_func_params;
	get_func_params << "(";
	get_func_params << "int p_PageIndex, int p_PageSize";
	//string search_key_params = print_cpp_search_key_params();
	//if (search_key_params != "" ) {
	//	get_func_params << ",\n";
	//	get_func_params << search_key_params;
	//}
	if (tableInfo_->has_search_key) {
		get_func_params << ",\n";
		get_func_params << print_cpp_search_key_params();
	}
	if (tableInfo_->nSessionParams) {
		//if(tableInfo_->has_search_key) {
			get_func_params << ",\n";
		//}
		get_func_params << print_cpp_session_params();
	}
	get_func_params << "\t\t);\n";
	get_func_signature << get_func_params.str();
	db_h << get_func_signature.str();
	db_h << format("boost::shared_ptr<Biz%1%> GetSingle%1%(int p_pkey);\n")
			% tableInfo_->tableName_;

	db_h << PrintGetSingleRecord_h();
	db_h << db_function_decls.str();
	db_h << boost::format("} /* close namespace %1% */ } /*close namespace db*/ } /* close namespace %2% */\n")
			% tableInfo_->tableName_
			% project_namespace ;
}

string PostgreSQLCodeGenerator::print_sp_pkey_param()
{
	return string (string("p_") + tableInfo_->vec_var_list[0]->var_name);
}

string PostgreSQLCodeGenerator::print_sp_pkey_field()
{
	return tableInfo_->vec_var_list[0]->var_name;
}

void PostgreSQLCodeGenerator::GenerateSelectSP()
{
	// search key params
	stringstream sp_decl;
	sp_decl << boost::format("function sp_%1%_select_%1%(\n")
		% tableInfo_->tableName_;
	sp_decl << "\tp_PageIndex  int,\n";
	sp_decl << "\tp_PageSize   int";
	if(tableInfo_->has_search_key){
		sp_decl << ",\n";
		sp_decl << print_sp_search_key_params();
	}
	if (tableInfo_->nSessionParams) {
		//if(tableInfo_->has_search_key) {
			sp_decl << ",\n";
		//}
		sp_decl << print_sp_session_params();
	}
	sp_decl << ")\n";
	stringstream sp_body;
	sp_body << "\nAS $$\nBEGIN\n"
		<< "\tRETURN QUERY SELECT * FROM(\n"
		<< "\t\tSELECT \n";
	struct var_list* v_ptr=tableInfo_->param_list;
	///stringstream select_clause, inner_join_clause, where_condition;
	stringstream sp_select_fields, sp_select_fields_with_type;
	print_sp_return_table_fields(sp_select_fields_with_type);
	sp_select_fields_with_type << ",\nr_RowNumber bigint\n";
	print_sp_select_fields(sp_select_fields);
	sp_body << sp_select_fields.str();
	//sp_body << print_sp_fields(SELECT);
	//fprintf(fptr, "\t\t\tRANK() OVER (ORDER BY %s ) AS RowNumber\n", tableInfo_->param_list->var_name.c_str());
	sp_body << "\t\t\t, RANK() OVER (ORDER BY\n";
	if (tableInfo_->has_search_key>0) {
		sp_body << print_sp_search_key_fields();
	} else {
		sp_body << "\t\t\t\t" << print_sp_pkey_field();
	}
	sp_body << "\n\t\t\t) AS RowNumber\n";
	sp_body << boost::format("\t\tFROM %1%\n")
			% tableInfo_->tableName_;
	sp_body << print_sp_select_inner_joins();
	// print out search keys
	if (tableInfo_->has_search_key || tableInfo_->nSessionParams) {
		sp_body << "\t\tWHERE ";
	}
	sp_body << print_sp_search_key_whereclause();
	sp_body << print_sp_session_key_whereclause();
	sp_body << boost::format ( "\t) sp_select_%s\n") 
		% tableInfo_->tableName_;
	sp_body << boost::format( "\tWHERE sp_select_%s.RowNumber BETWEEN (p_PageIndex*p_PageSize+1) AND ((p_PageIndex+1)*p_PageSize);\n")
		% tableInfo_->tableName_;
	sp_body << "END\n$$ LANGUAGE plpgsql;\n";
	/* output the sp to a file */
	string sp_select_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_select_postgres.sql"))); 
	std::ofstream select_sp(sp_select_fname.c_str(), ios_base::out|ios_base::trunc);
	if (!select_sp) {
		string err_msg="unable to open " + sp_select_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	select_sp << "create or replace " <<  sp_decl.str() 
		<< "\nRETURNS TABLE (\n"
		<< sp_select_fields_with_type.str() 
		<< "\n)"
		<< sp_body.str()
		<< endl
		<< endl;

	string sp_drop_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_select_")
					+ tableInfo_->tableName_ 
					+ string("_drop_postgres.sql"))); 
	std::ofstream drop_sp(sp_drop_fname.c_str(), ios_base::out|ios_base::trunc);
	if (!drop_sp) {
		string err_msg="unable to open " + sp_drop_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	drop_sp << "drop " << sp_decl.str() << ";"
		<< endl;

}



std::string PostgreSQLCodeGenerator::print_sp_search_key_params()
{
	stringstream search_key_params;
	struct var_list* v_ptr=tableInfo_->param_list;
	if (tableInfo_->has_search_key) {
		int count=0;
		bool print_comma = false;
		while (v_ptr) {
			if (v_ptr->options.search_key) {
				/*
				search_key_params <<  boost::format("\tp_%1% %2%") 
					% v_ptr->var_name.c_str()
					% print_sp_types(v_ptr->var_type);
				if (v_ptr->var_type==NVARCHAR_TYPE
					|| v_ptr->var_type==VARCHAR_TYPE 
					|| v_ptr->var_type==NCHAR_TYPE) {
					search_key_params << boost::format( "(%d)\n")
						% v_ptr->arr_len;
				}
				*/
				if (print_comma) {
					search_key_params << ",\n";
				}

				search_key_params <<  boost::format("\tp_%1% %2%") 
					% v_ptr->var_name.c_str()
					% v_ptr->print_sql_var_type();
				print_comma = true;
				/*
				++count;
				if (count < tableInfo_->has_search_key) {
					search_key_params <<  ",\n";
				} else {
					//search_key_params << "";
					break;
				}
				*/
			}

			if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
				struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr->options.ref_table_name)));
				if(tbl_ptr){
					tbl_ptr->dbCodeGenerator_->print_sp_search_key_params2(search_key_params,
							tbl_ptr->tableInfo_, print_comma);
				} else {
					search_key_params << format("referenced table: %1% not found in table list: ... exiting")
						% v_ptr->options.ref_table_name;
					exit(1);
				}
				//print_sp_select_params(fptr, with_pkey, rename_vars, v_ptr->var_name.c_str());
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 search_key_params << "\n";
	}
	return search_key_params.str();
}

std::string PostgreSQLCodeGenerator::print_sp_session_params()
{
	stringstream session_key_params;
	struct var_list* v_ptr=tableInfo_->param_list;
	if (tableInfo_->nSessionParams) {
		int count=0;
		while (v_ptr) {
			if (v_ptr->options.session) {
				session_key_params <<  boost::format("\tp_%1% %2%") 
					% v_ptr->var_name.c_str()
					% print_sp_types(v_ptr->var_type);
				if (v_ptr->var_type==NVARCHAR_TYPE
					|| v_ptr->var_type==VARCHAR_TYPE 
					|| v_ptr->var_type==NCHAR_TYPE) {
					session_key_params << boost::format( "(%d)\n")
						% v_ptr->arr_len;
				}
				++count;
				if (count<tableInfo_->nSessionParams) {
					session_key_params <<  ",\n";
				} else {
					//session_key_params << "\n";
					break;
				}
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 session_key_params << "\n";
	}
	return session_key_params.str();
}

std::string PostgreSQLCodeGenerator::print_sp_search_key_fields()
{
	stringstream search_key_fields_str;
	struct var_list* v_ptr=tableInfo_->param_list;
	if(tableInfo_->has_search_key){
		int count=0;
		while(v_ptr){
			if(v_ptr->options.search_key){
				search_key_fields_str <<  boost::format("\t\t\t\t %1%") 
					% v_ptr->var_name.c_str();
				++count;
				if(count<tableInfo_->has_search_key){
					search_key_fields_str <<  ",\n";
				} else 
					search_key_fields_str << "\n";
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 search_key_fields_str << "\n";
	}
	return search_key_fields_str.str();
}


string PostgreSQLCodeGenerator::print_sp_search_key_whereclause()
{
	stringstream search_key_where_clause_str;
	bool print_and = false;
	if (tableInfo_->has_search_key) {
		struct var_list* v_ptr = tableInfo_->param_list;
		//search_key_where_clause_str << "\t\tWHERE ";
		int count = 0;
		search_key_where_clause_str << "\n";
		while (v_ptr) {
			if(v_ptr->options.search_key){
				if (print_and) {
					search_key_where_clause_str << " AND \n";
				}
				boost::format("%s.%s ")
					% tableInfo_->tableName_.c_str()
					% v_ptr->var_name.c_str();
				if(isOfStringType(v_ptr->var_type)){
					search_key_where_clause_str << 
						boost::format("\t\t%1% like p_%1%")
						% v_ptr->var_name.c_str();
				} else {
					//fprintf(fptr, "= @%s", v_ptr->var_name.c_str());
					search_key_where_clause_str << 
						boost::format("\t\t%1% = p_%1%")
						% v_ptr->var_name.c_str();
				}
				print_and = true;
				//++count;
				//if(count<tableInfo_->has_search_key){
				//	search_key_where_clause_str << " AND \n";
				//}
			}
			if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
				struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr->options.ref_table_name)));
				if(tbl_ptr){
					tbl_ptr->dbCodeGenerator_->print_sp_search_key_whereclause2(search_key_where_clause_str,
							tbl_ptr->tableInfo_, print_and);
				} else {
					search_key_where_clause_str << format("referenced table: %1% not found in table list: ... exiting")
						% v_ptr->options.ref_table_name;
					exit(1);
				}
			}
			v_ptr=v_ptr->prev;
		}
	}
	return search_key_where_clause_str.str();
}


void PostgreSQLCodeGenerator::print_sp_search_key_whereclause2(stringstream & p_search_key_where_clause_str,
				TableInfoType * ptr_tableInfo, bool & print_and)
{
	stringstream search_key_where_clause_str;
	if(tableInfo_->has_search_key){
		struct var_list* v_ptr = tableInfo_->param_list;
		//int count = 0;
		while(v_ptr){
			if (v_ptr->options.search_key) {
				if (print_and) {
					p_search_key_where_clause_str << " AND \n";
				}
				boost::format("%s.%s ")
					% tableInfo_->tableName_.c_str()
					% v_ptr->var_name.c_str();
				if(isOfStringType(v_ptr->var_type)){
					p_search_key_where_clause_str << 
						boost::format("\t\t%1% like p_%1%")
						% v_ptr->var_name.c_str();
				} else {
					//fprintf(fptr, "= @%s", v_ptr->var_name.c_str());
					p_search_key_where_clause_str << 
						boost::format("\t\t%1% = p_%1%")
						% v_ptr->var_name.c_str();
				}
				print_and = true;
				// ++count;
				// if(count<tableInfo_->has_search_key){
				// 	search_key_where_clause_str << " AND \n";
				// }
			}
			v_ptr=v_ptr->prev;
		}
	}
	//return p_search_key_where_clause_str.str();
}

void PostgreSQLCodeGenerator::GenerateCreateSQL()
{
	using boost::format;
	stringstream create_sql_str;
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "ENTER");
	create_sql_str << format("CREATE TABLE %1% (\n")
			%  tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		string s;
		if (v_ptr->options.primary_key) {
			s = v_ptr->var_name + string(" serial unique");
		} else if(v_ptr->var_type == COMPOSITE_TYPE) {
			// skip
		} else {
			s= v_ptr->print_sql_var_decl();
			if (v_ptr->options.unique) {
				// cout << "var_name: " << v_ptr->var_name 
				// 	<< " has unique set"
				// 	<< endl;
				s += string(" unique");
			}
			if (v_ptr->options.ref_table_name != "") {
				s += string(" references ")
					+ v_ptr->options.ref_table_name 
					+ string ("(")
					+ v_ptr->options.ref_field_name
					+ string (")");
			}
		}
		if (s.length() > 0) {
			create_sql_str << "\t" << s;
		}
		v_ptr=v_ptr->prev;
		if (v_ptr && s.length() >0) {
			create_sql_str << ",\n";
		}
	}
	create_sql_str << "\n);\n";
	string sp_create_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_create_postgres.sql"))); 
	std::ofstream create_sp(sp_create_fname.c_str(), ios_base::out|ios_base::trunc);
	if(!create_sp){
		string err_msg="unable to open " + sp_create_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	create_sp << create_sql_str.str();
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "EXIT");
}



void PostgreSQLCodeGenerator::print_sp_select_fields(std::stringstream & p_sp_select_fields)
{
	// We will treat this function as recursion level 0
	using boost::format;
	struct var_list* v_ptr=tableInfo_->param_list;
	int recursion_level = 0;
	while(v_ptr){
		if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
			if (v_ptr->options.session) {
				// skip this field
				//print_comma = false;
				//p_sp_select_fields << "\t\t\t/* skipping :" << v_ptr->var_name 
				//	<< __LINE__ << ", " << __FILE__ << ", " << __PRETTY_FUNCTION__ 
				//	<< " */" 
				//	<< "\n";
				p_sp_select_fields << "\t\t\t" << v_ptr->var_name;
				if (v_ptr->prev) {
					p_sp_select_fields << ",\n";
				} else {
					p_sp_select_fields << "\n";
				}
				fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, " Use print_comma here and fix this function");
			} else if ( ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
			} else {
				p_sp_select_fields << boost::format("\t\t\t%1%.%2%") 
					% tableInfo_->tableName_
					% v_ptr->var_name;
				struct CppCodeGenerator* tbl_ptr = (dynamic_cast<CppCodeGenerator*>
						(TableCollectionSingleton::Instance()
							.my_find_table(v_ptr->options.ref_table_name)));
				if (tbl_ptr) {
					// p_sp_select_fields << ",\n";
					tbl_ptr->dbCodeGenerator_->print_sp_select_params(p_sp_select_fields,
							false, true, v_ptr->var_name, recursion_level+1);
					if (v_ptr->prev) {
						p_sp_select_fields << ",\n";
					}
				} else {
					p_sp_select_fields << format("referenced table: %1% not found in table list:  ... exiting")
						% v_ptr->options.ref_table_name;
					exit(1);
				}
				//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "there is definitely a bug here since i am not looking out if there is a comma needed");
			}
		} else if (v_ptr->var_type == COMPOSITE_TYPE) {
		} else {
			p_sp_select_fields <<  format("\t\t\t%1%")
				% v_ptr->var_name;
			if (v_ptr->prev) {
				p_sp_select_fields << ",\n";
			} else {
				p_sp_select_fields << "";
			}
		}
		v_ptr=v_ptr->prev;
	}
}


void PostgreSQLCodeGenerator::print_sp_return_table_fields(std::stringstream & p_sp_select_fields_with_type)
{
	using boost::format;
	struct var_list* v_ptr=tableInfo_->param_list;
	int recursion_level = 0;
	bool print_comma = false;
	while (v_ptr) {
		if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
			if (v_ptr->options.session) {
				p_sp_select_fields_with_type << "/*31*/ "
					<< v_ptr->print_sql_var_decl_for_select_return_table();
				print_comma = true;
			} else if ( ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
				//v_ptr = v_ptr->prev;
				//continue;
				p_sp_select_fields_with_type << "/* print_comma = false */ ";
				print_comma = false;
			} else {
				p_sp_select_fields_with_type << v_ptr->print_sql_var_decl_for_select_return_table();
				//if (v_ptr->prev) {
				//	p_sp_select_fields_with_type << "/*2*/\n";
				//}
				struct CppCodeGenerator* tbl_ptr = (dynamic_cast<CppCodeGenerator*>
						(TableCollectionSingleton::Instance()
							.my_find_table(v_ptr->options.ref_table_name)));
				if (tbl_ptr) {
					tbl_ptr->dbCodeGenerator_->print_sp_return_table_fields2(
							p_sp_select_fields_with_type
							, false, true, v_ptr->var_name, recursion_level);
					//if (v_ptr->prev) {
					//	p_sp_select_fields_with_type << "/*77*/ ,\n";
					//}
					print_comma = true;
				} else {
					cerr << format("referenced table: %1% not found in table list:  ... exiting")
						% v_ptr->options.ref_table_name;
					exit(1);
				}
				//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "there is definitely a bug here since i am not looking out if there is a comma needed");
				// p_sp_select_fields_with_type << "p_" << v_ptr->var_name;
				//p_sp_select_fields_with_type << v_ptr->print_sql_var_decl_for_select_return_table();
				//if (v_ptr->prev) {
				//	p_sp_select_fields_with_type << ",\n";
				//}
			}
		} else if (v_ptr->var_type==COMPOSITE_TYPE) { 
				//p_sp_select_fields_with_type << "/*22*/\n";
				print_comma = false;
		} else {
			p_sp_select_fields_with_type << "/*3*/ "
				<< v_ptr->print_sql_var_decl_for_select_return_table();
			//if (v_ptr->prev) {
			//	p_sp_select_fields_with_type << "/*1*/,\n";
			//} else {
			//	p_sp_select_fields_with_type << "";
			//}
			print_comma = true;
		}
		v_ptr=v_ptr->prev;
		if (v_ptr && print_comma) {
			p_sp_select_fields_with_type << "/*11*/,\n";
		}
	}
}


void PostgreSQLCodeGenerator::print_sp_select_params(std::stringstream & p_sp_select_fields,
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level)
{
	using boost::format;
	p_sp_select_fields <<  format("/*Entering: file: %7%, line: %6%, func: %5% called with params: %1% %2% %3% :recursion_level: %4%*/\n")
		% with_pkey % rename_vars % inner_join_tabname % recursion_level
		% __PRETTY_FUNCTION__ % __LINE__ % __FILE__
		;
	struct var_list* v_ptr=tableInfo_->param_list;
	p_sp_select_fields << "/*6*/,\n";
	if(!with_pkey){
		v_ptr=v_ptr->prev;
	}
	bool print_comma = false;
	while(v_ptr){
		if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
			p_sp_select_fields << format("\t\t\t/*7*/%1%.%2% as %1%_%2%")
				% tableInfo_->tableName_.c_str() % v_ptr->var_name ;
			struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
						(TableCollectionSingleton::Instance()
						 	.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				//p_sp_select_fields <<  ",\n";
				tbl_ptr->dbCodeGenerator_->print_sp_select_params(p_sp_select_fields,
						false, true, v_ptr->options.ref_table_name, recursion_level+1);
				print_comma = true;
			} else {
				p_sp_select_fields << format("referenced table: %1% not found in table list: ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
			//print_sp_select_params(fptr, with_pkey, rename_vars, v_ptr->var_name.c_str());
		}  else if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==true) {
			print_comma = false; // we want to skip this field
		} else {
			if(rename_vars){
				string orig_varname = inner_join_tabname;
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				p_sp_select_fields << format("\t\t\t%1%.%2% as %1%_%2%")
					% improved_name
					% v_ptr->var_name;
				print_comma = true;
			} else {
				p_sp_select_fields << format("\t\t\t%1%.%2%")
					% tableInfo_->tableName_
					% v_ptr->var_name;
				print_comma = true;
			}
		}
		v_ptr=v_ptr->prev;
		if(v_ptr && print_comma){
			p_sp_select_fields << "/*77*/,\n ";
			print_comma = false;
		}
	}
	//p_sp_select_fields << format( "/*Exiting print_sp_select_params called with params: %1% %2% %3% */\n")
	//		% with_pkey % rename_vars % inner_join_tabname;
}


void PostgreSQLCodeGenerator::print_sp_return_table_fields2(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level)
{
	using boost::format;
	//p_sp_select_fields <<  format("/*Entering print_sp_select_params called with params: %1% %2% %3% */\n")
	//	% with_pkey % rename_vars % inner_join_tabname;
	p_sp_select_fields_with_type << format("\n/* file: %1%, line: %2%: func: %3%*/\n") %
		__FILE__ % __LINE__ % __PRETTY_FUNCTION__ ;
	struct var_list* v_ptr=tableInfo_->param_list;
	p_sp_select_fields_with_type << "/*6*/ ,\n";
	if(!with_pkey){
		v_ptr=v_ptr->prev;
	}
	bool print_comma = false;
	while(v_ptr){
		if (v_ptr->options.many == false) {
			if(rename_vars){
				string orig_varname = inner_join_tabname;
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				p_sp_select_fields_with_type << 
					"/*44*/ r_" << improved_name << "_" << v_ptr->var_name << " " <<
					v_ptr->print_sql_var_type();
				print_comma = true;
			} else {
				p_sp_select_fields_with_type << 
					v_ptr->print_sql_var_decl_for_select_return_table();
				print_comma = true;
			}
		}
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
						(TableCollectionSingleton::Instance()
						 	.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				//p_sp_select_fields_with_type << ",\n";
				tbl_ptr->dbCodeGenerator_->print_sp_return_table_fields2(
						p_sp_select_fields_with_type
						, false, true, v_ptr->options.ref_table_name, recursion_level+1);
			} else {
				std::cerr << format("referenced table: %1% not found in table list: ... exiting\n")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
		} /*else { */
		/*} */
		v_ptr=v_ptr->prev;
		if (v_ptr && print_comma) {
			p_sp_select_fields_with_type << "/*5*/ ,\n";
			print_comma = false;
		} 
	}
	//p_sp_select_fields << format( "/*Exiting print_sp_select_params called with params: %1% %2% %3% */\n")
	//		% with_pkey % rename_vars % inner_join_tabname;
}

string PostgreSQLCodeGenerator::PrintCppSelectFunc()
{
	stringstream func_signature;
	using boost::format;
	func_signature << format("\n\nstd::vector<boost::shared_ptr<Biz%1%> > Get%1%") %
		tableInfo_->tableName_;
	stringstream func_params;
	func_params << "(";
	func_params << "int p_PageIndex, int p_PageSize";
	//string search_key_params = print_cpp_search_key_params();
	if (tableInfo_->has_search_key) {
		func_params << ",\n";
		func_params << print_cpp_search_key_params();
	}

	if (tableInfo_->nSessionParams) {
		//if(tableInfo_->has_search_key) {
			func_params << ",\n";
		//}
		func_params << print_cpp_session_params();
	}
	func_params << "\t\t)\n";
	func_signature << func_params.str();
	stringstream func_body;
	func_body << "{\n";
	func_body << "\tboost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());\n";
	func_body << boost::format("\tstd::vector<boost::shared_ptr<char> > char_ptr_vec(%1%);\n")
		% (tableInfo_->has_search_key + tableInfo_->nSessionParams + 2) /* int PageSize int PageIndex */ ;
	func_body << boost::format("\tconst char * paramValues[%1%];\n"
			"\tstd::stringstream ss_param_values[%1%];\n") 
		% (tableInfo_->has_search_key + tableInfo_->nSessionParams + 2) /* int PageSize int PageIndex */ ;
	int nActualParams = 0;
	func_body << format("\tss_param_values[%1%] << p_PageIndex;\n")
		% nActualParams;
	func_body << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
		% nActualParams;
	func_body << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
	func_body << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
				% nActualParams++;
	func_body << format("\tss_param_values[%1%] << p_PageSize;\n")
		% nActualParams;
	func_body << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
		% nActualParams;
	func_body << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
	func_body << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
				% nActualParams++;
	//int count;
	//struct var_list* v_ptr=tableInfo_->param_list;
	//while (v_ptr) {
	//	if (v_ptr->options.search_key) {
	//		func_body << boost::format("\tss_param_values[%1%] << p_%2%;\n")
	//			% nActualParams % v_ptr->var_name;
	//		func_body << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
	//			% nActualParams;
	//		func_body << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
	//		func_body << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
	//					% nActualParams++;
	//		++count;
	//	}
	//	v_ptr=v_ptr->prev;
	//}
	if (tableInfo_->has_search_key)
		func_body << print_cpp_sp_invoc_search_keys(nActualParams);
	if (tableInfo_->nSessionParams)
		func_body << print_cpp_sp_invoc_session_keys(nActualParams);

	func_body << format("\tstd::vector<boost::shared_ptr<Biz%1%> > vec_rval;\n") %
		tableInfo_->tableName_;
	func_body << boost::format("\tPGresult *res=PQexecParams(conn.get(), \n\t\t\"select * from sp_%1%_select_%1%($1::int, $2::int\"\n") %
		tableInfo_->tableName_;
	fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__,
	   "Move session variables printing outside has_search_key if condition");

	func_body << print_cpp_sp_invoc(nActualParams);

	// int count1=2;
	// if (tableInfo_->has_search_key) {
	// 	struct var_list* v_ptr1=tableInfo_->param_list;
	// 	func_body << "\t\t\t /*1*/ \",";
	// 	while (v_ptr1) {
	// 		if (v_ptr1->options.search_key) {
	// 			func_body << boost::format("$%1%::%2%")
	// 				% ++count1 % print_sp_types(v_ptr1->var_type);
	// 			if (count1 < tableInfo_->has_search_key + 2 ){
	// 				func_body << " /*2*/ , ";
	// 			} else {
	// 				break;
	// 			}
	// 		}
	// 		v_ptr1=v_ptr1->prev;
	// 	}
	// 	func_body << "\"\n";
	// }

	// int count2 = count1;
	// if (tableInfo_->nSessionParams) {
	// 	struct var_list * v_ptr1=tableInfo_->param_list;
	// 	func_body << "\t\t\t /*3*/ \",";
	// 	while (v_ptr1) {
	// 		if (v_ptr1->options.session) {
	// 			func_body << boost::format("$%1%::%2%")
	// 				% ++count2 % print_sp_types(v_ptr1->var_type);
	// 			if (count2 < tableInfo_->nSessionParams +2 + tableInfo_->has_search_key){
	// 				func_body << " /*4*/ , ";
	// 			} else {
	// 				break;
	// 			}
	// 		}
	// 		v_ptr1=v_ptr1->prev;
	// 	}
	// 	//func_body << boost::format(")\", %1%, NULL, paramValues, NULL, NULL,0);\n") %
	// 	//		nActualParams;
	// 	func_body << "\"\n";
	// } 
	// //else {
	// //if (tableInfo_->has_search_key || tableInfo_->nSessionParams) {
	// //	func_body << boost::format("\t\t\t)\", %1%, NULL, paramValues, NULL, NULL,0);\n") %
	// //			nActualParams;
	// //} else {
	// 	func_body << boost::format("\t\t\t\")\", %1%, NULL, paramValues, NULL, NULL,0);\n") %
	// 			nActualParams;
	// //}
	// //}

	func_body << "\tif (PQresultStatus(res) != PGRES_TUPLES_OK){\n";
	func_body << "\t\tint res_status = PQresultStatus(res);\n";
	func_body << "\t\tprintf(\"res_status=%d, PGRES_COMMAND_OK = %d, PGRES_TUPLES_OK=%d\\n\",\n"
				 << "\t\t\tres_status, PGRES_COMMAND_OK, PGRES_TUPLES_OK);\n";
	func_body << "\t\tfprintf(stderr, \"insert employee failed: %s\", PQerrorMessage(conn.get()));\n";
	func_body << "\t\tPQclear(res);\n";
	fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, 
		string("Fix me : exit_nicely may not be required since the PGconn has a custom deleter which closes the connection\n"));
	func_body << "\t\texit_nicely(conn.get());\n";
	func_body << "\t} else {\n";
	func_body << "\t	int nTuples = PQntuples(res);\n";
	/*
	func_body << "\t	int nFields = PQnfields(res);\n";
	func_body << "\t	printf( \"nTuples: %d, nFields=%d\\n\", nTuples, nFields);\n";
	func_body << "\t	for(int i=0; i<nFields; ++i){\n";
	func_body << "\t		char * fname=PQfname(res, i);\n";
	func_body << "\t		printf(\"fname: %s\\n\", fname);\n";
	func_body << "\t	}\n";
	func_body << "\t	//char * value=PQgetvalue(res, 0, 0);\n";
	func_body << "\t	//printf(\"value: %s\\n\", value);\n";
	*/

	// stringstream field_pos_stream;
	// print_cpp_select_field_positions(field_pos_stream);
	// field_pos_stream << "\t\tint32_t r_rownumber_fnum = PQfnumber(res, \"r_rownumber\");\n";
	// func_body << field_pos_stream.str();

	func_body << "\t\tfor (int i=0; i<nTuples; ++i) {\n";
	//stringstream convert_fields_str;
	//print_cpp_convert_db_fields_to_cpp(convert_fields_str);
	//func_body << convert_fields_str.str();
	func_body << format("\t\t\tusing %1%::db::%2%::Get%2%FromReader;\n")
			% project_namespace
			% tableInfo_->tableName_;
	func_body << format("\t\t\tvec_rval.push_back(Get%1%FromReader(res,i));\n")
			% tableInfo_->tableName_
			;
	func_body << "\t\t}\n";
	// This has to be fixed by using a shared pointer with 
	// a custom deleter - this works for now as the live server 
	// is leaking memory like a sieve
	func_body << "\t\tPQclear(res);\n";
	func_body << "\t}\n";
	func_body << "\treturn vec_rval;\n";
	func_body << "}\n";
	stringstream the_func;
	the_func << func_signature.str() 
		<< func_body.str()
		<< "\n";
	return the_func.str();
}

std::string PostgreSQLCodeGenerator::print_cpp_search_key_params()
{
	stringstream search_key_fields_str;
	struct var_list* v_ptr=tableInfo_->param_list;
	if(tableInfo_->has_search_key){
		int count=0;
		bool print_comma = false;
		while(v_ptr){
			// other complicated cases to be handled later - this is just a top level scan
			if (v_ptr->options.search_key && v_ptr->options.ref_table_name=="") {
				if (print_comma) {
					search_key_fields_str << ",\n";
				}
				search_key_fields_str <<  boost::format("\t\t");
				search_key_fields_str << print_cpp_types(v_ptr->var_type);
				search_key_fields_str <<  boost::format(" p_%1%") 
					% v_ptr->var_name;
				print_comma = true;
				//++count;
				//if(count<tableInfo_->has_search_key){
				//	search_key_fields_str <<  "/*11*/ ,\n";
				//} else {
				//	//search_key_fields_str << "\n";
				//	break;
				//}
			}
			if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
				struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr->options.ref_table_name)));
				if(tbl_ptr){
					tbl_ptr->dbCodeGenerator_->print_cpp_search_key_params2(search_key_fields_str,
							tbl_ptr->tableInfo_, print_comma);
				} else {
					search_key_fields_str << format("referenced table: %1% not found in table list: ... exiting")
						% v_ptr->options.ref_table_name;
					exit(1);
				}
				//print_sp_select_params(fptr, with_pkey, rename_vars, v_ptr->var_name.c_str());
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 search_key_fields_str << "";
	}
	return search_key_fields_str.str();
}

std::string PostgreSQLCodeGenerator::print_cpp_session_params()
{
	stringstream session_key_fields_str;
	struct var_list* v_ptr=tableInfo_->param_list;
	if(tableInfo_->nSessionParams){
		int count=0;
		while(v_ptr){
			if(v_ptr->options.session){
				session_key_fields_str <<  boost::format("\t\t");
				session_key_fields_str << print_cpp_types(v_ptr->var_type);
				session_key_fields_str <<  boost::format(" p_%1%") 
					% v_ptr->var_name;
				++count;
				if(count<tableInfo_->nSessionParams){
					session_key_fields_str <<  ",\n";
				} else {
					//session_key_fields_str << "\n";
					break;
				}
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 session_key_fields_str << "";
	}
	return session_key_fields_str.str();
}


/* This function and all similar functions have to be refactored */
void PostgreSQLCodeGenerator::print_cpp_select_field_positions(
				std::stringstream & p_sp_select_fields_with_type)
{
	using boost::format;
	int recursion_level=0;
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
			if (v_ptr->options.session) {
				p_sp_select_fields_with_type 
					<< "\t\t"
					<< v_ptr->print_cpp_var_type() 
					//<< "int32_t"
					<< " " << v_ptr->print_sql_var_name_for_select_return_table("") << "_fnum = " 
					<< "PQfnumber(res, \""
					<<  v_ptr->print_sql_var_name_for_select_return_table("") 
					<< "\");\n";
			} else if ( ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
				p_sp_select_fields_with_type 
					<< "\t\t"
					//<< v_ptr->print_cpp_var_type() 
					<< "int32_t"
					<< " " << v_ptr->print_sql_var_name_for_select_return_table("") << "_fnum = " 
					<< "PQfnumber(res, \""
					<<  v_ptr->print_sql_var_name_for_select_return_table("") 
					<< "\");\n";
				v_ptr=v_ptr->prev;
				continue;
			} else {
				p_sp_select_fields_with_type 
					<< "\t\t"
					<< "int32_t"
					<< " " << v_ptr->print_sql_var_name_for_select_return_table(string("")) << "_fnum = " 
					<< "PQfnumber(res, \""
					<< v_ptr->print_sql_var_name_for_select_return_table(string(""))  
					<< "\");\n";
				struct CppCodeGenerator* tbl_ptr = (dynamic_cast<CppCodeGenerator*>
						(TableCollectionSingleton::Instance()
							.my_find_table(v_ptr->options.ref_table_name)));
				if(tbl_ptr){
					tbl_ptr->dbCodeGenerator_->print_cpp_select_params(
							p_sp_select_fields_with_type
							, false, true, v_ptr->var_name, recursion_level+1);
				} else {
					cerr << format("referenced table: %1% not found in table list:  ... exiting")
						% v_ptr->options.ref_table_name;
					exit(1);
				}
				//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "there is definitely a bug here since i am not looking out if there is a comma needed");
				// p_sp_select_fields_with_type 
				// 	<< "\t\t"
				// 	<< "int32_t"
				// 	<< " " << v_ptr->print_sql_var_name_for_select_return_table(v_ptr->options.ref_table_name) << "_fnum = " 
				// 	<< "PQfnumber(res, \""
				// 	<< v_ptr->print_sql_var_name_for_select_return_table(v_ptr->options.ref_table_name)  
				// 	<< "\");\n";
			}
		} else {
			//p_sp_select_fields_with_type << v_ptr->print_sql_var_decl_for_select_return_table();

			p_sp_select_fields_with_type 
				<< "\t\t"
				//<< v_ptr->print_cpp_var_type() 
				<< "int32_t"
				<< " " << v_ptr->print_sql_var_name_for_select_return_table("") << "_fnum = " 
				<< "PQfnumber(res, \""
				<<  v_ptr->print_sql_var_name_for_select_return_table("") 
				<< "\");\n";
		}
		v_ptr=v_ptr->prev;
	}
}



void PostgreSQLCodeGenerator::print_cpp_select_params(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname,
		int recursion_level)
{
	using boost::format;
	p_sp_select_fields_with_type <<  format("/*Entering %4% called with params: %1% %2% %3% */\n")
		% with_pkey % rename_vars % inner_join_tabname % __PRETTY_FUNCTION__;
	struct var_list* v_ptr=tableInfo_->param_list;
	if(!with_pkey){
		p_sp_select_fields_with_type << "/* skipping field : " 
			<< v_ptr->var_name << " */" << endl;
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		if(rename_vars){
			string orig_varname = inner_join_tabname;
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			p_sp_select_fields_with_type 
				<< "\t\t"
				//<< v_ptr->print_cpp_var_type() 
				<< "int32_t"
				<< " r_" << improved_name << "_" << v_ptr->var_name << "_fnum = " 
				<< "PQfnumber(res, \""
				<< "r_" << improved_name << "_" << v_ptr->var_name 
				<< "\");\n";
		} else {
			p_sp_select_fields_with_type 
				<< "\t\t"
				//<< v_ptr->print_cpp_var_type() 
				<< "int32_t"
				<< v_ptr->print_sql_var_name_for_select_return_table(string("")) << "_fnum = " 
				<< "PQfnumber(res, \""
				<< v_ptr->print_sql_var_decl_for_select_return_table() 
				<< "\");\n";
		}
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
						(TableCollectionSingleton::Instance()
						 	.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				tbl_ptr->dbCodeGenerator_->print_cpp_select_params(
						p_sp_select_fields_with_type
						, false, true, v_ptr->options.ref_table_name, recursion_level+1);
			} else {
				cerr << format("referenced table: %1% not found in table list: ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
		} /*else {
			if(rename_vars){
				string orig_varname = inner_join_tabname;
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				p_sp_select_fields_with_type 
					<< "\t\t"
					//<< v_ptr->print_cpp_var_type() 
					<< "int32_t"
					<< " r_" << improved_name << "_" << v_ptr->var_name << "_fnum = " 
					<< "PQfnumber(res, \""
					<< "r_" << improved_name << "_" << v_ptr->var_name 
					<< "\");\n";
			} else {
				p_sp_select_fields_with_type 
					<< "\t\t"
					//<< v_ptr->print_cpp_var_type() 
					<< "int32_t"
					<< v_ptr->print_sql_var_name_for_select_return_table(string("")) << "_fnum = " 
					<< "PQfnumber(res, \""
					<< v_ptr->print_sql_var_decl_for_select_return_table() 
					<< "\");\n";
			}
		}
		*/
		v_ptr=v_ptr->prev;
		if(v_ptr){
			p_sp_select_fields_with_type << "";
		} 
	}
	//p_sp_select_fields << format( "/*Exiting print_sp_select_params called with params: %1% %2% %3% */\n")
	//		% with_pkey % rename_vars % inner_join_tabname;
}

void PostgreSQLCodeGenerator::print_cpp_convert_db_fields_to_cpp(std::stringstream & convert_fields_str)
{
	using boost::format;
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			struct CppCodeGenerator* tbl_ptr = (dynamic_cast<CppCodeGenerator*>
					(TableCollectionSingleton::Instance()
					 	.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				tbl_ptr->dbCodeGenerator_->print_cpp_convert_db_fields_to_cpp2(
						convert_fields_str
						, false, true, v_ptr->var_name);
				if (v_ptr->prev) {
					convert_fields_str << ";\n";
				}
			} else {
				convert_fields_str << format("referenced table: %1% not found in table list:  ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
			log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "there is definitely a bug here since i am not looking out if there is a comma needed");

			stringstream s1;
			

			convert_fields_str 
				<< "\t\t\t"
				<< v_ptr->print_cpp_var_type() 
				<< " " << v_ptr->print_sql_var_name_for_select_return_table("") << " = "
				<< "boost::lexical_cast< " << v_ptr->print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, i, "
				<< v_ptr->print_sql_var_name_for_select_return_table("")  << "_fnum "
				<< ") );\n";
			if (v_ptr->prev) {
				convert_fields_str << ",\n";
			}
		} else {
			convert_fields_str 
				<< "\t\t\t"
				<< v_ptr->print_cpp_var_type() 
				<< " " << v_ptr->print_sql_var_name_for_select_return_table("") << " = " 
				<< "boost::lexical_cast< " << v_ptr->print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, i, "
				<<  v_ptr->print_sql_var_name_for_select_return_table("") << "_fnum "
				<< ") );\n";
			if (v_ptr->prev) {
				convert_fields_str << "";
			} else {
				convert_fields_str << "";
			}
		}
		v_ptr=v_ptr->prev;
	}
}

void PostgreSQLCodeGenerator::print_cpp_convert_db_fields_to_cpp2(
		std::stringstream & p_sp_select_fields_with_type,
		bool with_pkey, bool rename_vars, string inner_join_tabname)
{
	using boost::format;
	//p_sp_select_fields <<  format("/*Entering print_sp_select_params called with params: %1% %2% %3% */\n")
	//	% with_pkey % rename_vars % inner_join_tabname;
	struct var_list* v_ptr=tableInfo_->param_list;
	if(!with_pkey){
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
						(TableCollectionSingleton::Instance()
						 	.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				tbl_ptr->dbCodeGenerator_->print_cpp_convert_db_fields_to_cpp2(
						p_sp_select_fields_with_type
						, false, true, v_ptr->options.ref_table_name);
			} else {
				cerr << format("referenced table: %1% not found in table list: ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
		} else {
			if(rename_vars){
				string orig_varname = inner_join_tabname;
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				p_sp_select_fields_with_type 
					<< "\t\t\t"
					<< v_ptr->print_cpp_var_type() 
					<< "int32_t"
					<< " r_" << improved_name << "_" << v_ptr->var_name << " = " 
					<< "PQfnumber(res, \""
					<< "r_" << improved_name << "_" << v_ptr->var_name 
					<< "\");\n";
			} else {
				p_sp_select_fields_with_type 
					<< "\t\t\t"
					//<< v_ptr->print_cpp_var_type() 
					<< "int32_t"
					<< v_ptr->print_sql_var_name_for_select_return_table("") << " = " 
					<< "PQfnumber(res, \""
					<< v_ptr->print_sql_var_decl_for_select_return_table() 
					<< "\");\n";
			}
		}
		v_ptr=v_ptr->prev;
		if(v_ptr){
			p_sp_select_fields_with_type << "";
		} 
	}
}

std::string PostgreSQLCodeGenerator::PrintGetSingleRecord_h()
{
	stringstream get_single_record_str;
	get_single_record_str << boost::format("\nboost::shared_ptr<Biz%1%> Get%1%FromReader(PGresult * res, int row);\n")
		% tableInfo_->tableName_; 
	return get_single_record_str.str();
}

// This function probably needs a helper function which has a vector<string> 
// reference parameter which stores the recursively traversed bll
// objects on this (using it like a stack)
std::string PostgreSQLCodeGenerator::PrintGetSingleRecord()
{
	using boost::format;
	stringstream get_single_record_str;
	//get_single_record_str << boost::format("/* file: %1%, line: %2%: func: %3% : tableName_: %4%*/\n") %
	//	__FILE__ % __LINE__ % __PRETTY_FUNCTION__  % tableInfo_ -> tableName_;
	get_single_record_str << boost::format("\nboost::shared_ptr<Biz%1%> Get%1%FromReader(PGresult * res, int row)\n")
		% tableInfo_->tableName_; 
	get_single_record_str << "{\n";
	vector<boost::shared_ptr<stringstream> > vec_reader_str;
	boost::shared_ptr<stringstream> dummy(new stringstream);
	vec_reader_str.push_back(dummy);

	std::stringstream dummy_defns;
	int recursion_level = 0;
	if(tableInfo_->param_list){
		stringstream field_pos_stream;
		/* I have to fix a bug at this point */
		print_cpp_select_field_positions(field_pos_stream);
		field_pos_stream << "\t\tint32_t r_rownumber_fnum = PQfnumber(res, \"r_rownumber\");\n";
		get_single_record_str << field_pos_stream.str();
		struct var_list* v_ptr=tableInfo_->param_list;
		// This is for the Object within this current object 
		while (v_ptr) {
			if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
				if (v_ptr->options.session) {
				} else if ( ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
				} else {
					string orig_varname (v_ptr->var_name);
					int pos = orig_varname.find("_Code");
					string improved_name = orig_varname.substr(0, pos);
					if (vec_reader_str.size() <= recursion_level +1) {
						boost::shared_ptr<stringstream> dummy(new stringstream);
						vec_reader_str.push_back(dummy);
					}
					(*vec_reader_str[recursion_level+1]) <<  format("\t\t/*4*/boost::shared_ptr<Biz%1%> l_biz_%2%(new Biz%1%(\n")
						% v_ptr->options.ref_table_name
						% improved_name ;
					
					//vec_reader_str[recursion_level] << print_reader_param_with_cast(v_ptr, v_ptr->options.ref_table_name);
					(*vec_reader_str[recursion_level+1]) << "\t\t\t/*3*/" << print_reader_param_with_cast(v_ptr, string(""));
					(*vec_reader_str[recursion_level+1]) << ",\n";
					struct CppCodeGenerator * t_ptr = (dynamic_cast<CppCodeGenerator*>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr->options.ref_table_name)));
					(*vec_reader_str[recursion_level+1]) << "/* calling print_reader : from line : " << __LINE__ << ", func:" << __PRETTY_FUNCTION__ 
						<< "tableName_: "  << tableInfo_->tableName_  << " */" << endl ;
					t_ptr->dbCodeGenerator_->print_reader(false, true,  v_ptr->options.ref_table_name, vec_reader_str, recursion_level+1, dummy_defns, true);
					(*vec_reader_str[recursion_level+1]) << "\t\t\t));\n";
				}
			} else if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==true) {
				// (*vec_reader_str[recursion_level]) << "UNHANDLED CASE: line: " << __LINE__
				// 		<< ", file: " << __FILE__ 
				// 		<< ", function: " << __PRETTY_FUNCTION__ 
				// 		<< endl;
				string orig_varname (v_ptr->var_name);
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				if (vec_reader_str.size() <= recursion_level +1) {
					boost::shared_ptr<stringstream> dummy(new stringstream);
					vec_reader_str.push_back(dummy);
				}
				get_single_record_str << boost::format("/* file dummy: %1%, line: %2%: func: %3%*/\n") %
					__FILE__ % __LINE__ % __PRETTY_FUNCTION__ ;
				(*vec_reader_str[recursion_level+1]) <<  format("\t\t/*4*/std::vector<boost::shared_ptr<Biz%1%> > l_dummy_biz_vec_%2%;\n")
					% v_ptr->options.ref_table_name
					% improved_name ;
			}
			v_ptr=v_ptr->prev;
		}

		(*vec_reader_str[recursion_level]) << format("\t/*5*/boost::shared_ptr<Biz%1%> l_biz_%1% (new Biz%1%(\n")
			% tableInfo_->tableName_;
		//cout << "calling print_reader : from line : " << __LINE__ << ", func:" << __PRETTY_FUNCTION__ << "tableName_: "  << tableInfo_->tableName_ << endl;
		print_reader(true, true,  "", vec_reader_str, recursion_level, dummy_defns, false);

		(*vec_reader_str[recursion_level]) <<  "\t\t));\n";

		(*vec_reader_str[recursion_level]) <<  format("\treturn l_biz_%1%;\n")
			% tableInfo_->tableName_;
	}
	(*vec_reader_str[recursion_level]) << "}\n\n";

	get_single_record_str << "\n/* dummy_defns */\n" << dummy_defns.str() << endl;
	for(int i=vec_reader_str.size()-1; i>=0; --i) {
		get_single_record_str << (*vec_reader_str[i]).str();
	}
	return get_single_record_str.str();
}

std::string PostgreSQLCodeGenerator::print_reader_param_with_cast(var_list* v_ptr, std::string ref_table_name)
{
	// this first variable 
	std::stringstream s;
	using boost::format;

	// s	<< "\t\t\tboost::lexical_cast< " << v_ptr->print_cpp_var_type() << " > ("
	// 	<< "PQgetvalue(res, row, ";
	// if(ref_table_name==""){
	// 	s <<  "r_" << v_ptr->var_name << "_fnum ) )" ;
	// } else {
	// 	string orig_varname = v_ptr->var_name.c_str();
	// 	int pos = orig_varname.find("_Code");
	// 	string improved_name = orig_varname.substr(0, pos);
	// 	s << "r_" << improved_name << "_" << v_ptr->var_name << "_fnum) )";
	// }
	s << v_ptr->print_psql_to_cpp_conversion(ref_table_name);
	return s.str();
}


void PostgreSQLCodeGenerator::print_reader(bool with_pkey, bool rename_vars, std::string inner_join_tabname,
		std::vector<boost::shared_ptr<std::stringstream> >& p_vec_reader_str, int recursion_level,
		std::stringstream & dummy_defns,
		bool descend)
{

	(*p_vec_reader_str[recursion_level] ) << boost::format("/* file: %1%, line: %2%: func: %3%: recursion_level: %4%, inner_join_tabname: %5%*/\n") %
		__FILE__ % __LINE__ % __PRETTY_FUNCTION__ % recursion_level % inner_join_tabname;
	using boost::format;
	stringstream s;
	struct var_list * v_ptr=tableInfo_->param_list;
	if(!with_pkey){
		v_ptr=v_ptr->prev;
	}
	// cout << "/*: file: "  << __FILE__  << ", line: " << __LINE__ << ", func: " << __PRETTY_FUNCTION__ 
	// 	<< " recursion_level: " << recursion_level 
	// 	<< " tableName_: " << tableInfo_->tableName_
	// 	<< "*/\n"
	// 	<< endl;
	if (recursion_level > 10 ) {
		cerr << " recursion_level too high - probably a runaway ... exiting: FILE: " 
			<< __FILE__
			<< ", line: " << __LINE__ 
			<< ", func: " << __PRETTY_FUNCTION__ 
			;
		exit(1);
	}
		
	while (v_ptr) {
		if (v_ptr->options.many==false) {
			if (v_ptr->options.ref_table_name!="") {
				if (v_ptr->options.session) {
					if(rename_vars){
						string orig_varname = inner_join_tabname;
						int pos = orig_varname.find("_Code");
						string improved_name = orig_varname.substr(0, pos);
						s << "\t\t/*111*/ " << v_ptr->print_psql_to_cpp_conversion(improved_name);
					}else {
						s << "\t\t/*222*/ " << v_ptr->print_psql_to_cpp_conversion(string(""));
					}
				} else if ( ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
					s << "\t\t/*2*/ " << v_ptr->print_psql_to_cpp_conversion(string(""));
				} else {
					struct CppCodeGenerator* t_ptr = (dynamic_cast<CppCodeGenerator*>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr->options.ref_table_name)));
					if (t_ptr==0) {
						s << format("table: %1% not found: line: %2%, file: %3%\n")
								% v_ptr->options.ref_table_name % __LINE__ % __FILE__;
					} else {
						string orig_varname = v_ptr->var_name.c_str();
						int pos = orig_varname.find("_Code");
						string improved_name = orig_varname.substr(0, pos);
						s << format("\t\t\tl_biz_%1%") % improved_name;
						if (descend) {
							string orig_varname = v_ptr->var_name.c_str();
							int pos = orig_varname.find("_Code");
							string improved_name = orig_varname.substr(0, pos);
							boost::shared_ptr<stringstream> dummy(new stringstream);
							p_vec_reader_str.push_back(dummy);
							(*p_vec_reader_str[recursion_level+1]) << format("\t\t/*4*/boost::shared_ptr<Biz%1%> l_biz_%2%(new Biz%1%(\n")
								% v_ptr->options.ref_table_name
								% improved_name ;
							/* fix problem here: inner_join_tabname has _Code in it - figure out where it comes from */
							(*p_vec_reader_str[recursion_level+1]) << "\t\t/*1*/ " 
								<< v_ptr->print_psql_to_cpp_conversion(inner_join_tabname) 
								<< ",";
							t_ptr->dbCodeGenerator_->print_reader(false, true,  v_ptr->options.ref_table_name, p_vec_reader_str
									, recursion_level+1, dummy_defns, descend /* which must be true*/ );
							(*p_vec_reader_str[recursion_level+1]) << "));/* close */\n";
						}
					}
				}
			} else {
				if(rename_vars){
					string orig_varname = inner_join_tabname;
					int pos = orig_varname.find("_Code");
					string improved_name = orig_varname.substr(0, pos);
					s << "\t\t/*1*/ " << v_ptr->print_psql_to_cpp_conversion(improved_name);
				}else {
					s << "\t\t/*2*/ " << v_ptr->print_psql_to_cpp_conversion(string(""));
				}
			}
		} else {
			// the list would have been constructed - just pass it to the constructor
			s << format("\t\tl_dummy_biz_vec_%1%") % v_ptr->var_name;
			string orig_varname (v_ptr->var_name);
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			dummy_defns << "/* recursion_level: " << recursion_level << " */\n" ;
			if (recursion_level > 0) {
				// otherwise it would be printed from the lowest level table
				dummy_defns <<  format("\t\t/*44*/std::vector<boost::shared_ptr<Biz%1%> > l_dummy_biz_vec_%2%;\n")
						% v_ptr->options.ref_table_name
						% improved_name ;
			}
		}
		v_ptr=v_ptr->prev;
		if(v_ptr) {
			// fprintf(edit_out, ",\n");
			s << ",\n";
		}
	}
	//if(rename_vars==true)
	//	fprintf(edit_out, "\n\t\t\t/* exiting ... print_reader called with rename_vars=true*/\n");
	(*p_vec_reader_str[recursion_level]) <<  s.str();
}

std::string PostgreSQLCodeGenerator::GenerateRandomData()
{
	using boost::format;
	stringstream s;
	struct var_list * v_ptr=tableInfo_->param_list;
	// skip primary key
	v_ptr=v_ptr->prev;
	//s << "/*: file: "  << __FILE__  << ", line: " << __LINE__ << ", func: " << __PRETTY_FUNCTION__ 
	//	<< "*/\n"
	//	<< endl;
	//
	s << "insert into " << tableInfo_->tableName_ << "(";
	int counter=0;
	bool print_comma = false;
	while (v_ptr) {
		if (v_ptr->var_type == COMPOSITE_TYPE) { 
		} else {
			s << "\t" << v_ptr->var_name;
			print_comma=true;
		}
		v_ptr = v_ptr->prev;
		if (v_ptr && print_comma) {
			s << ",\n";
			print_comma = false;
		} else {
		}
	}
	s << "\t) values (\n";
	if (tableInfo_->tab_options.is_master_tables_list == true) {
		stringstream final;
		for(int i=0; i<TableCollectionSingleton::Instance().Tables.size(); ++i) {
			final << s.str() << "'" 
				<< TableCollectionSingleton::Instance().Tables[i]->tableInfo_->tableName_ 
				<< "'"
				<< ");\n";
		}
		string sp_random_data_fname (string(outputDirPrefix_.c_str()
						+ string("/sp_")
						+ tableInfo_->tableName_ 
						+ string("_random_data_postgres.sql"))); 
		std::ofstream random_data_sp(sp_random_data_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!random_data_sp){
			string err_msg="unable to open " + sp_random_data_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		random_data_sp << "/*tabIndex_->tab_options.is_master_tables_list is SET*/" << endl;
		random_data_sp << final.str();
		return final.str();
	} else if (tableInfo_->tab_options.is_role_table == true) {
		stringstream final;
		for(int i=0; i<TableCollectionSingleton::Instance().Tables.size(); ++i) {
			for(int j=0; j<3; ++j) {
				string perm;
				if ( j==0 ) {
					perm = "View";
				} else if (j==1) { 
					perm = "Add";
				} else if (j==2) {
					perm = "Edit";
				}
				final << s.str() << "'" 
					<< (TableCollectionSingleton::Instance().Tables[i]->tableInfo_->tableName_ + string(":")
							+ perm)
					<< "'"
					<< ");\n";
			}
		}
		string sp_random_data_fname (string(outputDirPrefix_.c_str()
						+ string("/sp_")
						+ tableInfo_->tableName_ 
						+ string("_random_data_postgres.sql"))); 
		std::ofstream random_data_sp(sp_random_data_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!random_data_sp){
			string err_msg="unable to open " + sp_random_data_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		random_data_sp << "/*tabIndex_->tab_options.is_role_table is SET*/" << endl;
		random_data_sp << final.str();
		return final.str();
	} else if (tableInfo_->tableName_ == "Employee") {
		stringstream final;
		stringstream emp;
		emp << "'Zenon', 'Andrew', 'DSouza', '1', 'Mr', 'Z', '13-jan-1976', 0, '1-may-2010');\n";
		final << s.str() << emp.str();

		emp.str("");
		emp << "'Atul', 'Francis', 'DSouza', '1', 'Mr', 'Atul', '9-dec-1979', 0, '1-dec-2010');\n";
		final << s.str() << emp.str();

		emp.str("");
		emp << "'Michael', '', 'DSouza', '1', 'Mr', 'Mike', '12-mar-1968', 0, '1-may-2010');\n";
		final << s.str() << emp.str();

		emp.str("");
		emp << "'Neil', 'Xavier', 'DSouza', '1', 'Mr', 'NxD', '24-dec-1977', 0, '1-may-2010');\n";
		final << s.str() << emp.str();

		emp.str("");
		emp << "'Veera', 'Olivia', 'DSouza', '0', 'Ms', 'V', '25-nov-1981', 0, '1-may-2010');\n";
		final << s.str() << emp.str();

		emp.str("");
		emp << "'Chanchal', 'Singh', 'DSouza', '0', 'Ms', 'chanchu', '4-feb-1978', 0, '1-dec-2010');\n";
		final << s.str() << emp.str();

		emp.str("");
		emp << "'Wilbur', '', 'DSouza', '1', 'Mr', 'W', '25-feb-1979', 0, '1-dec-2010');\n";
		final << s.str() << emp.str();

		emp.str("");
		emp << "'Abhishek', 'V', 'Chetnani', '1', 'Mr', 'A', '14-jul-1978', 0, '1-jan-2011');\n";
		final << s.str() << emp.str();

		string sp_random_data_fname (string(outputDirPrefix_.c_str()
						+ string("/sp_")
						+ tableInfo_->tableName_ 
						+ string("_random_data_postgres.sql"))); 
		std::ofstream random_data_sp(sp_random_data_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!random_data_sp){
			string err_msg="unable to open " + sp_random_data_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		random_data_sp << final.str();
		return final.str();
	} else if (tableInfo_->tableName_ == "User_Login") {
		stringstream final;
		stringstream user_login;
		user_login << "'zenond', 'zenond123', 1);\n";
		final << s.str() << user_login.str();
		user_login.str("");

		user_login << "'atuld', 'atul123', 2);\n";
		final << s.str() << user_login.str();
		user_login.str("");

		user_login << "'michaeld', 'michael123', 3);\n";
		final << s.str() << user_login.str();
		user_login.str("");

		user_login << "'nxd', 'nxd123', 4);\n";
		final << s.str() << user_login.str();
		user_login.str("");

		user_login << "'veerad', 'veerad123', 5);\n";
		final << s.str() << user_login.str();
		user_login.str("");

		user_login << "'chanchud', 'chanchud', 6);\n";
		final << s.str() << user_login.str();
		user_login.str("");

		user_login << "'wilburd', 'wilburd123', 7);\n";
		final << s.str() << user_login.str();
		user_login.str("");

		user_login << "'abhishekc', 'abhishekc123', 8);\n";
		final << s.str() << user_login.str();
		user_login.str("");
		string sp_random_data_fname (string(outputDirPrefix_.c_str()
						+ string("/sp_")
						+ tableInfo_->tableName_ 
						+ string("_random_data_postgres.sql"))); 
		std::ofstream random_data_sp(sp_random_data_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!random_data_sp){
			string err_msg="unable to open " + sp_random_data_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		random_data_sp << final.str();

		return final.str();
	} else if (tableInfo_->tableName_ == "User_Role") {
		stringstream final;
		stringstream user_role;

		user_role << "1, 1);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "1, 2);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "1, 3);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 5);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 6);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 7);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 8);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 9);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 10);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 11);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 12);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 13);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 14);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 15);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 16);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 17);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 18);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 19);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 20);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 21);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "6, 22);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 23);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 24);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 25);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 26);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 27);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 28);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 29);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 30);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 31);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 32);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 33);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "7, 34);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 35);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 36);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 37);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 38);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 39);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 40);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 41);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 42);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 43);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 44);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 45);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 46);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 47);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 48);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 49);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 50);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 51);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 52);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 53);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 54);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "8, 55);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 62);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 63);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 64);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 65);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 66);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 67);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 68);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 69);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 70);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 71);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 72);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		user_role << "5, 73);\n";
		final << s.str() << user_role.str();
		user_role.str("");

		string sp_random_data_fname (string(outputDirPrefix_.c_str()
						+ string("/sp_")
						+ tableInfo_->tableName_ 
						+ string("_random_data_postgres.sql"))); 
		std::ofstream random_data_sp(sp_random_data_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!random_data_sp){
			string err_msg="unable to open " + sp_random_data_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		random_data_sp << final.str();

		return final.str();

	} else {
		int nRecords=50;
		stringstream final;
		for (int i=0; i<nRecords; ++i) {
			stringstream random_data_varying;
			v_ptr = tableInfo_->param_list->prev; // skip primary key
			bool print_comma = false;
			while (v_ptr) {
				if (v_ptr->var_type == COMPOSITE_TYPE) { 
					print_comma = false;
				} else {
					random_data_varying << "\t" << v_ptr->print_random_value(i, nRecords);
					print_comma = true;
				}
				v_ptr = v_ptr->prev;
				if (v_ptr && print_comma) {
					random_data_varying << ",\n";
					print_comma = false;
				} else {
				}
			}
			random_data_varying << ");\n";
			final << s.str() << random_data_varying.str();
		}

		string sp_random_data_fname (string(outputDirPrefix_.c_str()
						+ string("/sp_")
						+ tableInfo_->tableName_ 
						+ string("_random_data_postgres.sql"))); 
		std::ofstream random_data_sp(sp_random_data_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!random_data_sp){
			string err_msg="unable to open " + sp_random_data_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		random_data_sp << final.str();
		return final.str();
	}

}

std::string PostgreSQLCodeGenerator::print_sp_select_inner_joins()
{
	stringstream inner_join_str;
	struct var_list * v_ptr = tableInfo_->param_list;
	v_ptr = tableInfo_->param_list;
	int loop_counter = 0;
	int recursion_level = 0;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			if (v_ptr->options.session) {
			} else if ( ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
			} else {
				string orig_varname = v_ptr->var_name;
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos) ;
				inner_join_str << boost::format(
					"\t\tINNER JOIN %1% %2% ON %3%.%4%=%5%.%6% \n") %
					v_ptr->options.ref_table_name %
					improved_name %
					tableInfo_->tableName_%
					v_ptr->var_name %
					improved_name %
					v_ptr->options.ref_field_name;
				struct CppCodeGenerator* tbl_ptr = (dynamic_cast<CppCodeGenerator*>
						(TableCollectionSingleton::Instance()
							.my_find_table(v_ptr->options.ref_table_name)));
				if(tbl_ptr){
					tbl_ptr->dbCodeGenerator_->print_sp_select_inner_joins2(inner_join_str,
							false, true, v_ptr->var_name, recursion_level+1);
				} else {
					cerr << format("referenced table: %1% not found in table list:  ... exiting")
						% v_ptr->options.ref_table_name;
					exit(1);
				}
			}
		}
		v_ptr=v_ptr->prev;
		++loop_counter;
	}
	return inner_join_str.str();
}

void PostgreSQLCodeGenerator::print_sp_select_inner_joins2(stringstream & p_inner_join_str,
		bool with_pkey, bool rename_vars, string inner_join_tabname, int recursion_level)
{
	p_inner_join_str << boost::format("/* file: %1%, line: %2%: func: %3% with params */\n") %
		__FILE__ % __LINE__ % __PRETTY_FUNCTION__ ;
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			string orig_varname = v_ptr->var_name;
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos) ;
			p_inner_join_str << boost::format(
				"\t\tINNER JOIN %1% %2% ON %3%.%4%=%5%.%6% \n") %
				v_ptr->options.ref_table_name %
				improved_name %
				tableInfo_->tableName_%
				v_ptr->var_name %
				improved_name %
				v_ptr->options.ref_field_name;
			struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
						(TableCollectionSingleton::Instance()
						 	.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				tbl_ptr->dbCodeGenerator_->print_sp_select_inner_joins2(
						p_inner_join_str
						, false, true, v_ptr->options.ref_table_name, recursion_level+1);
			} else {
				cerr << format("referenced table: %1% not found in table list: ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
		} 
		v_ptr=v_ptr->prev;
		//if(v_ptr){
		//	p_inner_join_str << "";
		//} 
	}
}

std::string PostgreSQLCodeGenerator::print_cpp_sp_invoc_search_keys(int & nActualParams)
{
	struct var_list* v_ptr=tableInfo_->param_list;
	stringstream search_key_param_setup_str;
	bool print_comma = false;

	while (v_ptr) {
		if (v_ptr->options.search_key) {
			search_key_param_setup_str << boost::format("\tss_param_values[%1%] << p_%2%;\n")
				% nActualParams % v_ptr->var_name;
			search_key_param_setup_str << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
				% nActualParams;
			search_key_param_setup_str << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
			search_key_param_setup_str << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
						% nActualParams++;
		}
		if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
			struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
						(TableCollectionSingleton::Instance()
							.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				tbl_ptr->dbCodeGenerator_->print_cpp_sp_invoc_search_keys2(search_key_param_setup_str,
						tbl_ptr->tableInfo_, print_comma, nActualParams);
			} else {
				search_key_param_setup_str << format("referenced table: %1% not found in table list: ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
			//print_sp_select_params(fptr, with_pkey, rename_vars, v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
	}
	return search_key_param_setup_str.str();
}


std::string PostgreSQLCodeGenerator::print_cpp_sp_invoc_session_keys(int & nActualParams)
{
	struct var_list* v_ptr=tableInfo_->param_list;
	stringstream session_key_param_setup_str;
	while (v_ptr) {
		if (v_ptr->options.session) {
			session_key_param_setup_str << boost::format("\tss_param_values[%1%] << p_%2%;\n")
				% nActualParams % v_ptr->var_name;
			session_key_param_setup_str << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
				% nActualParams;
			session_key_param_setup_str << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
			session_key_param_setup_str << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
						% nActualParams++;
		}
		v_ptr=v_ptr->prev;
	}
	return session_key_param_setup_str.str();
}


void PostgreSQLCodeGenerator::GenerateSelectSingleRecordSP()
{
	// search key params
	stringstream sp_decl;
	sp_decl << boost::format("function sp_%1%_select_single_%1%(\n")
		% tableInfo_->tableName_;
	sp_decl << "\tp_pkey  int\n";

	sp_decl << ")\n";
	stringstream sp_body;
	sp_body << "\nAS $$\nBEGIN\n"
		<< "\tRETURN QUERY SELECT * FROM(\n"
		<< "\tSELECT \n";
	struct var_list* v_ptr=tableInfo_->param_list;
	stringstream sp_select_fields, sp_select_fields_with_type;
	print_sp_return_table_fields(sp_select_fields_with_type);
	print_sp_select_fields(sp_select_fields);
	sp_body << sp_select_fields.str();
	sp_body << boost::format("\t\tFROM %1%\n")
			% tableInfo_->tableName_;
	sp_body << print_sp_select_inner_joins();
	// print out search keys
	sp_body << boost::format( "\tWHERE  %1%.%1%_Code = p_pkey\n")
		% tableInfo_->tableName_;
	sp_body << boost::format ( "\t) sp_select_%s;\n") 
		% tableInfo_->tableName_;
	sp_body << "END\n$$ LANGUAGE plpgsql;\n";

	/* output the sp to a file */
	string sp_select_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_select_single_postgres.sql"))); 
	std::ofstream select_sp(sp_select_fname.c_str(), ios_base::out|ios_base::trunc);
	if (!select_sp) {
		string err_msg="unable to open " + sp_select_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	select_sp << "create or replace " << sp_decl.str() 
		<< "\nRETURNS TABLE (\n"
		<< sp_select_fields_with_type.str() 
		<< "\n)"
		<< sp_body.str()
		<< endl
		<< endl;

	/* output the sp to a file */
	string sp_drop_select_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_drop_select_single_postgres.sql"))); 
	std::ofstream drop_select_sp(sp_drop_select_fname.c_str(), ios_base::out|ios_base::trunc);
	if (!drop_select_sp) {
		string err_msg="unable to open " + sp_drop_select_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	drop_select_sp << "DROP " << sp_decl.str() << ";"
		<< endl;

}


string PostgreSQLCodeGenerator::PrintCppSelectSingleFunc()
{
	stringstream func_signature;
	using boost::format;
	func_signature << format("\n\nboost::shared_ptr<Biz%1%> GetSingle%1%") %
		tableInfo_->tableName_;
	stringstream func_params;
	func_params << "(";
	func_params << "int p_pkey)\n";
	func_signature << func_params.str();
	stringstream func_body;
	func_body << "{\n";
	func_body << "\tboost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());\n";
	func_body << boost::format("\tstd::vector<boost::shared_ptr<char> > char_ptr_vec(%1%);\n")
		% (1) /* int PageSize int PageIndex */ ;
	func_body << boost::format("\tconst char * paramValues[%1%];\n"
			"\tstd::stringstream ss_param_values[%1%];\n") 
		% (1) /* int PageSize int PageIndex */ ;
	int nActualParams = 0;
	func_body << format("\tss_param_values[%1%] << p_pkey;\n")
		% nActualParams;

	func_body << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
		% nActualParams;
	func_body << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
	func_body << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
				% nActualParams++;

	func_body << format("\t boost::shared_ptr<Biz%1%>  rval;\n") %
		tableInfo_->tableName_;
	func_body << boost::format("\tPGresult *res=PQexecParams(conn.get(), \n\t\t\"select * from sp_%1%_select_single_%1%($1::int\"") %
		tableInfo_->tableName_;

	func_body << boost::format("\t\t\t\")\", %1%, NULL, paramValues, NULL, NULL,0);\n") %
			nActualParams;

	func_body << "\tif (PQresultStatus(res) != PGRES_TUPLES_OK){\n";
	func_body << "\t\tint res_status = PQresultStatus(res);\n";
	func_body << "\t\tprintf(\"res_status=%d, PGRES_COMMAND_OK = %d, PGRES_TUPLES_OK=%d\\n\",\n"
				 << "\t\t\tres_status, PGRES_COMMAND_OK, PGRES_TUPLES_OK);\n";
	func_body << format("\t\tfprintf(stderr, \"select single %1% failed: %%s\", PQerrorMessage(conn.get()));\n") 
			% tableInfo_->tableName_;
	func_body << "\t\tPQclear(res);\n";
	fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, 
		string("Fix me : exit_nicely may not be required since the PGconn has a custom deleter which closes the connection\n"));
	func_body << "\t\texit_nicely(conn.get());\n";
	func_body << "\t} else {\n";
	func_body << "\t	int nTuples = PQntuples(res);\n";
	// Need to do an assert here
	fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, 
		string("Fix me : assert here that we have only one record retured from the database\n"));

	/*
	func_body << "\t	int nFields = PQnfields(res);\n";
	func_body << "\t	printf( \"nTuples: %d, nFields=%d\\n\", nTuples, nFields);\n";
	func_body << "\t	for(int i=0; i<nFields; ++i){\n";
	func_body << "\t		char * fname=PQfname(res, i);\n";
	func_body << "\t		printf(\"fname: %s\\n\", fname);\n";
	func_body << "\t	}\n";
	func_body << "\t	//char * value=PQgetvalue(res, 0, 0);\n";
	func_body << "\t	//printf(\"value: %s\\n\", value);\n";
	*/

	func_body << format("\t\tusing %1%::db::%2%::Get%2%FromReader;\n")
			% project_namespace
			% tableInfo_->tableName_;
	func_body << format("\t\tboost::shared_ptr<Biz%1%> rval ( Get%1%FromReader(res, 0) );\n")
			% tableInfo_->tableName_;
	// This has to be fixed by using a shared pointer with 
	// a custom deleter - this works for now as the live server 
	// is leaking memory like a sieve
	func_body << "\t\treturn rval;\n";
	func_body << "\t\tPQclear(res);\n";
	func_body << "\t}\n";
	func_body << format("\treturn boost::shared_ptr<Biz%1%>();\n") % tableInfo_->tableName_;
	func_body << "}\n";
	stringstream the_func;
	the_func << func_signature.str() 
		<< func_body.str()
		<< "\n";
	return the_func.str();
}


void PostgreSQLCodeGenerator::GenerateAuthenticateLoginSP()
{
	using boost::format;
	string sp_authenticate_login_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_authenticate_login_postgres.sql"))); 
	std::ofstream authenticate_login_sp(sp_authenticate_login_fname.c_str(), ios_base::out|ios_base::trunc);
	if(!authenticate_login_sp){
		string err_msg="unable to open " + sp_authenticate_login_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	stringstream authenticate_login_sp_str;
	authenticate_login_sp_str << boost::format("create or replace function sp_%1%_authenticate_login(")
		% tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			authenticate_login_sp_str << format("\tp_%1% ") %
					v_ptr->var_name;
			authenticate_login_sp_str << v_ptr->print_sql_var_type()
				<< endl;
		} else if (v_ptr->options.is_login_password_field) {
			// if the password field occurs after the username 
			// field in the input - the code generated here will be wrong
			authenticate_login_sp_str << format("\t, p_%1% ") %
					v_ptr->var_name;
			authenticate_login_sp_str<< v_ptr->print_sql_var_type()
				<< endl;
		}
		v_ptr=v_ptr->prev;
	}
	authenticate_login_sp_str << "\t, out r_user_login_code int)\nas $$\nbegin\n";
	authenticate_login_sp_str << format("\tselect user_login_code into r_user_login_code from %1% where ")
		% tableInfo_->tableName_ ;

	v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			authenticate_login_sp_str << format("\t%1% = p_%1% ") %
					v_ptr->var_name;
		} else if (v_ptr->options.is_login_password_field) {
			// if the password field occurs after the username 
			// field in the input - the code generated here will be wrong
			authenticate_login_sp_str << format("\tand %1% = p_%1% ") %
					v_ptr->var_name;
		}
		v_ptr=v_ptr->prev;
	}
	authenticate_login_sp_str << ";\nend\n$$ LANGUAGE plpgsql;\n";
	authenticate_login_sp << authenticate_login_sp_str.str();
}

void PostgreSQLCodeGenerator::GenerateCppAuthenticateLogin(std::stringstream & p_func_decl,
			std::stringstream & p_func_defn)
{
	stringstream cpp_auth_login_str;
	using boost::format;
	stringstream func_signature;
	func_signature << format("int32_t %1%_Authenticate_User(") %
		tableInfo_->tableName_;
	
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			func_signature << format("\t%2% p_%1% ") %
					v_ptr->var_name % v_ptr->print_cpp_var_type();
		} else if (v_ptr->options.is_login_password_field) {
			// if the password field occurs after the username 
			// field in the input - the code generated here will be wrong
			func_signature << ",\n";
			func_signature << format("\t%2% p_%1%") %
					v_ptr->var_name % v_ptr->print_cpp_var_type();
		}
		v_ptr=v_ptr->prev;
	}

	func_signature << ")";
	stringstream func_body;
	func_body << "{\n";
	func_body << "	boost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());\n";
	func_body << "	std::vector<boost::shared_ptr<char> > char_ptr_vec(2);\n";
	func_body << "	const char * paramValues[2];\n";
	func_body << "	std::stringstream ss_param_values[2];\n";
	v_ptr=tableInfo_->param_list;
	int count = 0;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			func_body << format("\tss_param_values[%1%] << p_%2%;\n") %
					count % v_ptr->var_name;
			func_body << format("boost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
					% count;
			func_body << format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % count;
			func_body << format("\tparamValues[%1%]=s_ptr%1%.get();\n") % count++;
		} else if (v_ptr->options.is_login_password_field) {
			// if the password field occurs after the username 
			// field in the input - the code generated here will be wrong
			func_body << format("\tss_param_values[%1%] << p_%2%;\n") %
					count % v_ptr->var_name;
			func_body << format("boost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
					% count;
			func_body << format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % count;
			func_body << format("\tparamValues[%1%]=s_ptr%1%.get();\n") % count++;
		}
		v_ptr=v_ptr->prev;
	}
	func_body << "\t// 3rd param is the output parameter \n";

	func_body << format("\tPGresult *res=PQexecParams(conn.get(), "
		"\"select * from sp_%1%_authenticate_login($1::varchar, $2::varchar)\"\n"
			"\t\t, 2, NULL, paramValues, NULL, NULL, 0);\n")
			% tableInfo_->tableName_ ;


	func_body << "	if (PQresultStatus(res) != PGRES_TUPLES_OK){\n";
	func_body << "		int res_status = PQresultStatus(res);\n";
	func_body << "		printf(\"res_status=%d, PGRES_COMMAND_OK = %d, PGRES_TUPLES_OK=%d\\n\",\n";
	func_body << "			res_status, PGRES_COMMAND_OK, PGRES_TUPLES_OK);\n";
	func_body << format("		fprintf(stderr, \"sp_%1%_authenticate_login failed to execute: %%s\", PQerrorMessage(conn.get()));\n")
			% tableInfo_->tableName_ ;
	func_body << "		PQclear(res);\n";
	func_body << "		exit_nicely(conn.get());\n";
	func_body << "\t\treturn false;\n";
	func_body << "	} else {\n";
	//func_body << "	int32_t valid_login_fnum = PQfnumber(res, \"valid_login\");\n";
	//func_body << "\t\tint32_t valid_login = boost::lexical_cast<int32_t> (PQgetvalue(res, 0, valid_login_fnum) );\n";
	//func_body << "\t\tprintf(\"valid_login=%d\\n\", valid_login);\n";
	//func_body << "\t\treturn valid_login==1 ? true:false;\n";

	v_ptr=tableInfo_->param_list;
	func_body << format("		int nTuples = PQntuples(res);\n");
	func_body << format("		if (nTuples==0) {\n");
	func_body << format("			return 0;\n");
	func_body << format("		}\n");
	func_body << format("		int32_t %1%_fnum = PQfnumber(res, \"r_%1%\");\n") % v_ptr->var_name;
	func_body << format("		int32_t %1% = boost::lexical_cast<int32_t> (PQgetvalue(res, 0, %1%_fnum) );\n") % v_ptr->var_name;
	func_body << format("		printf(\"%1%=%%d\\n\", %1%);\n") % v_ptr->var_name;
	func_body << format("		return %1%;\n") % v_ptr->var_name;
	func_body << "\t}\n";


	func_body << "}\n\n";
	p_func_decl << func_signature.str() << ";\n";
	p_func_defn << func_signature.str() << "\n" << func_body.str();
	//AddFuncDecl(func_signature.str() + string(";\n"));

}

void PostgreSQLCodeGenerator::AddFuncDecl(std::string func_decl)
{
	db_function_decls << func_decl;
}


std::string PostgreSQLCodeGenerator::print_sp_session_key_whereclause()
{
	stringstream session_key_where_clause_str;
	if(tableInfo_->nSessionParams){
		struct var_list* v_ptr = tableInfo_->param_list;
		//session_key_where_clause_str << "\t\tWHERE ";
		int count = 0;
		while(v_ptr){
			if(v_ptr->options.session){
				boost::format("%s.%s ")
					% tableInfo_->tableName_.c_str()
					% v_ptr->var_name.c_str();
				if(isOfStringType(v_ptr->var_type)){
					session_key_where_clause_str << 
						boost::format("%1% like p_%1%")
						% v_ptr->var_name.c_str();
				} else {
					//fprintf(fptr, "= @%s", v_ptr->var_name.c_str());
					session_key_where_clause_str << 
						boost::format("%1% = p_%1%")
						% v_ptr->var_name.c_str();
				}
				++count;
				if(count<tableInfo_->nSessionParams){
					session_key_where_clause_str << " AND \n";
				}
			}
			v_ptr=v_ptr->prev;
		}
	}
	return session_key_where_clause_str.str();
}

// we follow only one level of references for search keys: If something else
// is needed you have to modify this function to allow for recursion 
void PostgreSQLCodeGenerator::print_sp_search_key_params2(stringstream & p_search_key_params,
				TableInfoType * ptr_tableInfo, bool & print_comma)
{
	struct var_list* v_ptr = ptr_tableInfo->param_list;
	//bool print_comma = false;
	while (v_ptr) {
		if (v_ptr->options.search_key) {
			/*
			p_search_key_params <<  boost::format("\tp_%1% %2%") 
				% v_ptr->var_name.c_str()
				% print_sp_types(v_ptr->var_type);
			if (v_ptr->var_type==NVARCHAR_TYPE
				|| v_ptr->var_type==VARCHAR_TYPE 
				|| v_ptr->var_type==NCHAR_TYPE) {
				search_key_params << boost::format( "(%d)\n")
					% v_ptr->arr_len;
			}
			*/
			if (print_comma) {
				p_search_key_params << ",\n";
			}
			p_search_key_params << boost::format("\tp_%1% %2%") 
				% v_ptr->var_name.c_str()
				% v_ptr->print_sql_var_type();
			print_comma = true;
			/*
			++count;
			if (count<ptr_tableInfo->has_search_key) {
				p_search_key_params <<  ",\n";
			} else {
				//search_key_params << "";
				break;
			}
			*/
		}
		v_ptr = v_ptr->prev;
	}
}


// we follow only one level of references for search keys: If something else
// is needed you have to modify this function to allow for recursion 
void PostgreSQLCodeGenerator::print_cpp_search_key_params2(stringstream & p_search_key_params,
				TableInfoType * ptr_tableInfo, bool & print_comma)
{
	struct var_list* v_ptr = ptr_tableInfo->param_list;
	//bool print_comma = false;
	while (v_ptr) {
		if (v_ptr->options.search_key) {
			if (print_comma) {
				p_search_key_params << ",\n";
			}

			p_search_key_params <<  boost::format("\t\t");
			p_search_key_params << print_cpp_types(v_ptr->var_type);
			p_search_key_params <<  boost::format(" p_%1%") 
				% v_ptr->var_name;
			print_comma = true;
		}
		v_ptr = v_ptr->prev;
	}
}

void PostgreSQLCodeGenerator::print_cpp_sp_invoc_search_keys2(stringstream & p_search_key_param,
				TableInfoType * ptr_tableInfo, bool & print_comma, int & nActualParams)
{
	struct var_list* v_ptr=ptr_tableInfo->param_list;
	while (v_ptr) {
		if (v_ptr->options.search_key) {
			p_search_key_param << boost::format("\tss_param_values[%1%] << p_%2%;\n")
				% nActualParams % v_ptr->var_name;
			p_search_key_param << boost::format("\tboost::shared_ptr<char> s_ptr%1%(strdup(ss_param_values[%1%].str().c_str()), MallocDeleter());\n")
				% nActualParams;
			p_search_key_param << boost::format("\tchar_ptr_vec.push_back(s_ptr%1%);\n") % nActualParams;
			p_search_key_param << boost::format("\tparamValues[%1%]=s_ptr%1%.get();\n")
						% nActualParams++;
		}
		v_ptr=v_ptr->prev;
	}
}

std::string PostgreSQLCodeGenerator::print_cpp_sp_invoc(int nActualParams)
{
	stringstream sp_invoc_str;
	int count1=2;
	if (tableInfo_->has_search_key) {
		struct var_list* v_ptr1=tableInfo_->param_list;
		sp_invoc_str << "\t\t\t /*1*/ \",";
		bool print_comma = false;
		while (v_ptr1) {
			if (v_ptr1->options.search_key) {
				if (print_comma) {
					sp_invoc_str << ", ";
				}
				sp_invoc_str << boost::format("$%1%::%2%")
					% ++count1 % print_sp_types(v_ptr1->var_type);
				// if (count1 < tableInfo_->has_search_key + 2 ){
				// 	sp_invoc_str << " /*2*/ , ";
				// } else {
				// 	break;
				// }
				print_comma  = true;
			}
			if (v_ptr1->options.ref_table_name!="" && v_ptr1->options.many==false) {
				struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr1->options.ref_table_name)));
				if(tbl_ptr){
					tbl_ptr->dbCodeGenerator_->print_cpp_sp_search_invoc2(sp_invoc_str,
							tbl_ptr->tableInfo_, print_comma, count1);
				} else {
					sp_invoc_str << format("referenced table: %1% not found in table list: ... exiting")
						% v_ptr1->options.ref_table_name;
					exit(1);
				}
				//print_sp_select_params(fptr, with_pkey, rename_vars, v_ptr->var_name.c_str());
			}
			v_ptr1=v_ptr1->prev;
		}
		sp_invoc_str << "\"\n";
	}

	int count2 = count1;
	if (tableInfo_->nSessionParams) {
		struct var_list * v_ptr1=tableInfo_->param_list;
		sp_invoc_str << "\t\t\t /*3*/ \",";
		while (v_ptr1) {
			if (v_ptr1->options.session) {
				sp_invoc_str << boost::format("$%1%::%2%")
					% ++count2 % print_sp_types(v_ptr1->var_type);
				if (count2 < tableInfo_->nSessionParams +2 + tableInfo_->has_search_key){
					sp_invoc_str << " /*4*/ , ";
				} else {
					break;
				}
			}
			v_ptr1=v_ptr1->prev;
		}
		sp_invoc_str << "\"\n";
	} 
	sp_invoc_str << boost::format("\t\t\t\")\", %1%, NULL, paramValues, NULL, NULL,0);\n") %
			nActualParams;
	return sp_invoc_str.str();
}

void PostgreSQLCodeGenerator::print_cpp_sp_search_invoc2(stringstream & sp_invoc_str,
	TableInfoType *	ptr_tableInfo, bool & print_comma, int & count1)
{
	struct var_list* v_ptr1=tableInfo_->param_list;
	while (v_ptr1) {
		if (v_ptr1->options.search_key) {
			if (print_comma) {
				sp_invoc_str << ", ";
			}
			sp_invoc_str << boost::format("$%1%::%2%")
				% ++count1 % print_sp_types(v_ptr1->var_type);
			print_comma  = true;
		}
		v_ptr1=v_ptr1->prev;
	}
}
