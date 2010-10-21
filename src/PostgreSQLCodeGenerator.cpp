#include "std_headers.h"
#include "error.h"
#include "utils.h"
#include "CppCodeGenerator.h"

#include "PostgreSQLCodeGeneratorFactory.h"
#include "PostgreSQLCodeGenerator.h"
#include "TableCollectionSingleton.hpp"

#include "std_using.h"

using boost::format;
using boost::io::group;
extern char project_namespace[];

PostgreSQLCodeGenerator::PostgreSQLCodeGenerator(TableInfoType * p_tabInfo,
						 std::string & p_output_dir_path)
	//: tableInfo_(p_tabInfo),
	//  outputDirPrefix_(p_output_dir_path)			
	:AbstractDataBaseCodeGenerator(p_tabInfo, p_output_dir_path)
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
	GenerateSelectSP();
	GenerateCreateSQL();
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
	insert_sp << print_sp_param_decls( INSERT);
	insert_sp << ") AS $$\n";
	insert_sp << "BEGIN\n";
	insert_sp << "\tINSERT INTO " << tableInfo_->tableName_ 
		<< "(";
	insert_sp << print_sp_fields(  INSERT);
	insert_sp << "\t\t) values (";
	insert_sp << print_sp_params(  INSERT);
	insert_sp << "\t\t);\n";
	insert_sp << "\tselect last_value into ";
	print_sp_1st_param( insert_sp, INSERT);
	insert_sp << boost::format(" from %1%_%2%_seq;\n")
		% tableInfo_->tableName_ % print_sp_pkey_field();
	insert_sp << endl << "END\n$$ LANGUAGE plpgsql;\n";
	if(insert_sp)
		insert_sp.close();
			
			
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
		}
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
		//fprintf(fptr, "\t@%s ", v_ptr->var_name.c_str());
		if(v_ptr->var_type == COMPOSITE_TYPE) {
			v_ptr=v_ptr->prev;
			continue;
		}
		sp_params << tab_indent << boost::format("p_%1%") % v_ptr->var_name;
		v_ptr=v_ptr->prev;
		if(v_ptr){
			//fprintf(fptr, ",\n");
			sp_params << ",\n";
		} else {
			//fprintf(fptr, "\n");
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
		// because of the Window function rank() ... over order by ...
		// we need the extra comma
		//if(v_ptr){
		//	sp_fields << ",\n";
		//} else {
		//	sp_fields << "\n";
		//}
		sp_fields << ",\n";
	}
	return sp_fields.str();
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
	PrintMallocDeleter(cpp_db_impl);
	print_exit_nicely(cpp_db_impl);
	PrintGetConn(cpp_db_impl);
	PrintCppInsertFunc(cpp_db_impl);

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
	cpp_db_impl << boost::format("#include <cstring>\n");
	cpp_db_impl << boost::format("#include <sstream>\n");
	cpp_db_impl << boost::format("#include \"%1%_bll.h\"\n") % tableInfo_->tableName_;

	cpp_db_impl << boost::format("namespace %1% { namespace db { namespace %2% {\n")
			% project_namespace % tableInfo_->tableName_;

}

void PostgreSQLCodeGenerator::PrintCppInsertFunc(ofstream & cpp_db_impl)
{
	cpp_db_impl << boost::format("int Insert%1%(Biz%1% & %2%)\n{") % tableInfo_->tableName_
		% print_lower_table_name();
	//print_lower_fname(cpp_db_impl);
	//cpp_db_impl << ")\n{\n";
	cpp_db_impl << "\tboost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());\n";
	cpp_db_impl << boost::format("\tstd::vector<boost::shared_ptr<char> > char_ptr_vec(%1%);\n")
		% tableInfo_->vec_var_list.size() ;

	//const char *paramValues[2];
	//int			paramLengths[2];
	//int			paramFormats[2];
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
	cpp_db_impl << "\t	int nTuples = PQntuples(res);\n";
	cpp_db_impl << "\t	int nFields = PQnfields(res);\n";
	cpp_db_impl << "\t	printf( \"nTuples: %d, nFields=%d\\n\", nTuples, nFields);\n";
	cpp_db_impl << "\t	for(int i=0; i<nFields; ++i){\n";
	cpp_db_impl << "\t		char * fname=PQfname(res, i);\n";
	cpp_db_impl << "\t		printf(\"fname: %s\\n\", fname);\n";
	cpp_db_impl << "\t	}\n";
	cpp_db_impl << "\t	char * value=PQgetvalue(res, 0, 0);\n";
	cpp_db_impl << "\t	printf(\"value: %s\\n\", value);\n";
	cpp_db_impl << "\t}\n";




	cpp_db_impl << "\n}\n";
}

void PostgreSQLCodeGenerator::PrintGetConn(ofstream & cpp_db_impl)
{
	cpp_db_impl << "\nPGconn * GetPGConn()\n{\n";

	cpp_db_impl << "\tconst char  *conninfo;\n";
	cpp_db_impl << "\tPGconn      *conn;\n";
	cpp_db_impl << "\tconninfo = \"dbname=nxd port=5433\";\n";

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
	sp_decl << boost::format("create or replace function sp_%1%_select_%1%(\n")
		% tableInfo_->tableName_;

	sp_decl << "\tp_PageIndex  int,\n";
	sp_decl << "\tp_PageSize   int";
	

	if(tableInfo_->has_search_key){
		sp_decl << ",\n";
		sp_decl << print_sp_search_key_params();
	}

	sp_decl << ")\n";

	stringstream sp_body;
	sp_body << "\nAS $$\nBEGIN\n"
		<< "\tRETURN QUERY SELECT * FROM(\n"
		<< "\t\tSELECT \n";
	struct var_list* v_ptr=tableInfo_->param_list;
	///stringstream select_clause, inner_join_clause, where_condition;
	stringstream sp_select_fields, sp_select_fields_with_type;
	print_sp_select_fields(sp_select_fields, sp_select_fields_with_type);
	sp_select_fields_with_type << ",\np_RowNumber bigint\n";
	sp_body << sp_select_fields.str();
	//sp_body << print_sp_fields(SELECT);
	//fprintf(fptr, "\t\t\tRANK() OVER (ORDER BY %s ) AS RowNumber\n", tableInfo_->param_list->var_name.c_str());
	sp_body << "\t\t\tRANK() OVER (ORDER BY\n";
	if (tableInfo_->has_search_key>0) {
		sp_body << print_sp_search_key_fields();
	} else {
		sp_body << "\t\t\t\t" << print_sp_pkey_field();
	}
	sp_body << "\n\t\t\t) AS RowNumber\n";
	v_ptr=tableInfo_->param_list;
	int loop_counter=0;
	while(v_ptr){
		if(loop_counter==0)
			sp_body << boost::format("\t\tFROM %1%\n")
				% tableInfo_->tableName_;
		else if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos) ;

			sp_body << boost::format(
				"\t\tINNER JOIN %1% %2% ON %3%.%4%=%5%.%6% \n") %
				v_ptr->options.ref_table_name %
				improved_name %
				tableInfo_->tableName_%
				v_ptr->var_name %
				improved_name %
				v_ptr->options.ref_field_name;

		}
		v_ptr=v_ptr->prev;
		++loop_counter;
	}
	// print out search keys
	sp_body << print_sp_search_key_whereclause();

	sp_body << boost::format ( "\t) sp_select_%s\n") 
		% tableInfo_->tableName_;
	sp_body << boost::format( "\tWHERE sp_select_%s.RowNumber BETWEEN (p_PageIndex*p_PageSize+1) AND ((p_PageIndex+1)*p_PageSize);\n")
		% tableInfo_->tableName_;
	sp_body << "END\n$$ LANGUAGE plpgsql;\n";


	string sp_select_fname (string(outputDirPrefix_.c_str()
					+ string("/sp_")
					+ tableInfo_->tableName_ 
					+ string("_select_postgres.sql"))); 
	std::ofstream select_sp(sp_select_fname.c_str(), ios_base::out|ios_base::trunc);
	if (!select_sp) {
		string err_msg="unable to open " + sp_select_fname + "for writing";
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
	}
	select_sp << sp_decl.str() 
		<< "\nRETURNS TABLE (\n"
		<< sp_select_fields_with_type.str() 
		<< "\n)"
		<< sp_body.str()
		<< endl
		<< endl;
}



std::string PostgreSQLCodeGenerator::print_sp_search_key_params()
{
	stringstream search_key_params;
	struct var_list* v_ptr=tableInfo_->param_list;
	if (tableInfo_->has_search_key) {
		int count=0;
		while (v_ptr) {
			if (v_ptr->options.search_key) {
				search_key_params <<  boost::format("\tp_%1% %2%") 
					% v_ptr->var_name.c_str()
					% print_sp_types(v_ptr->var_type);
				if (v_ptr->var_type==NVARCHAR_TYPE
					|| v_ptr->var_type==VARCHAR_TYPE 
					|| v_ptr->var_type==NCHAR_TYPE) {
					search_key_params << boost::format( "(%d)\n")
						% v_ptr->arr_len;
				}
				++count;
				if (count<tableInfo_->has_search_key) {
					search_key_params <<  ",\n";
				} else 
					search_key_params << "\n";
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 search_key_params << "\n";
	}
	return search_key_params.str();
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
	if(tableInfo_->has_search_key){
		struct var_list* v_ptr = tableInfo_->param_list;
		search_key_where_clause_str << "\t\tWHERE ";
		int count = 0;
		while(v_ptr){
			if(v_ptr->options.search_key){
				boost::format("%s.%s ")
					% tableInfo_->tableName_.c_str()
					% v_ptr->var_name.c_str();
				if(isOfStringType(v_ptr->var_type)){
					search_key_where_clause_str << 
						boost::format("%1% like p_%1%")
						% v_ptr->var_name.c_str();
				} else {
					//fprintf(fptr, "= @%s", v_ptr->var_name.c_str());
					search_key_where_clause_str << 
						boost::format("%1% = p_%1%")
						% v_ptr->var_name.c_str();
				}
				++count;
				if(count<tableInfo_->has_search_key){
					search_key_where_clause_str << " AND \n";
				}
			}
			v_ptr=v_ptr->prev;
		}
	}
	return search_key_where_clause_str.str();
}

void PostgreSQLCodeGenerator::GenerateCreateSQL()
{
	using boost::format;
	stringstream create_sql_str;
	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "ENTER");
	
	create_sql_str << format("CREATE TABLE %1% (\n")
			%  tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		string s;
		if (v_ptr->options.primary_key) {
			s = v_ptr->var_name + string(" serial unique");
		} else {
			s= v_ptr->print_sql_var_decl();
			if (v_ptr->options.unique) {
				cout << "var_name: " << v_ptr->var_name 
					<< " has unique set"
					<< endl;
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
	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, "EXIT");
}



void PostgreSQLCodeGenerator::print_sp_select_fields(std::stringstream & p_sp_select_fields,
				std::stringstream & p_sp_select_fields_with_type)
{
	using boost::format;
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			struct CppCodeGenerator* tbl_ptr = (dynamic_cast<CppCodeGenerator*>
					(TableCollectionSingleton::Instance()
					 	.my_find_table(v_ptr->options.ref_table_name)));
			if(tbl_ptr){
				tbl_ptr->dbCodeGenerator_->print_sp_select_params(p_sp_select_fields,
						p_sp_select_fields_with_type
						, false, true, v_ptr->var_name.c_str());
				if (v_ptr->prev) {
					p_sp_select_fields << ",\n";
					p_sp_select_fields_with_type << ",\n";
				}
			} else {
				p_sp_select_fields << format("referenced table: %1% not found in table list:  ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
			p_sp_select_fields << boost::format("\t\t\t%1%.%2%,\n") 
				% tableInfo_->tableName_
				% v_ptr->var_name;
		} else {
			p_sp_select_fields <<  format("\t\t\t%1%,\n")
				% v_ptr->var_name;
			p_sp_select_fields_with_type << v_ptr->print_sql_var_decl_for_select_return_table();
			if (v_ptr->prev) {
				p_sp_select_fields_with_type << ",\n";
			} else {
				p_sp_select_fields_with_type << "";
			}
		}
		v_ptr=v_ptr->prev;
	}
}


void PostgreSQLCodeGenerator::print_sp_select_params(std::stringstream & p_sp_select_fields,
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
				tbl_ptr->dbCodeGenerator_->print_sp_select_params(p_sp_select_fields,
						p_sp_select_fields_with_type
						, false, true, v_ptr->options.ref_table_name);
			} else {
				p_sp_select_fields << format("referenced table: %1% not found in table list: ... exiting")
					% v_ptr->options.ref_table_name;
				exit(1);
			}
			if(tbl_ptr){
				p_sp_select_fields <<  ",";
			}
			p_sp_select_fields << format("\t\t\t%1%.%2%,\n")
				% tableInfo_->tableName_.c_str() % v_ptr->var_name ;
			//print_sp_select_params(fptr, with_pkey, rename_vars, v_ptr->var_name.c_str());
		} else {
			if(rename_vars){
				string orig_varname = inner_join_tabname;
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				p_sp_select_fields << format("\t\t\t%1%.%2% as %1%_%2%")
					% improved_name
					% v_ptr->var_name;
				p_sp_select_fields_with_type << 
					"p_" << improved_name << "_" << v_ptr->var_name << " " <<
					v_ptr->print_sql_var_type();
			} else {
				p_sp_select_fields << format("\t\t\t%1%.%2%")
					% tableInfo_->tableName_
					% v_ptr->var_name;
				p_sp_select_fields_with_type << 
					v_ptr->print_sql_var_decl_for_select_return_table();
			}
		}
		
		v_ptr=v_ptr->prev;
		if(v_ptr){
			p_sp_select_fields << ",\n ";
			p_sp_select_fields_with_type << ",\n";
		} 
	}
	//p_sp_select_fields << format( "/*Exiting print_sp_select_params called with params: %1% %2% %3% */\n")
	//		% with_pkey % rename_vars % inner_join_tabname;
}
