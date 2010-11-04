/*
 *  tree.C implementation of tree structures for parsed grammar
 *  Copyright (C) 2008, 2009 Neil Xavier D'Souza
 *  Released under GNU GPL Version 2
 */

#include "tree.h"
#include "utils.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <time.h>
extern std::vector<std::string> dict;


TableInfoType::TableInfoType(string name, struct var_list*  elist,
		vector<var_list*> & p_vec_var_list)
	: 
	tableName_(name), param_list(elist), table_scope(0), has_composite_objs(0),
	has_multi(0), has_search_key(0), nInvisible(0),
	vec_var_list(p_vec_var_list)
{
	printf("ENTER: %s: tableName_: %s\n", __PRETTY_FUNCTION__, tableName_.c_str());
	
	struct var_list* v_ptr = param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name != ""){
			++has_composite_objs;
			if(v_ptr->options.many){
				++has_multi;
			}
		}
		if(v_ptr->options.search_key){
			++has_search_key;
		}
		if (v_ptr->options.visible==false) {
			++nInvisible ;
		}
		v_ptr=v_ptr->prev;
	}
	printf("EXIT %s: tableName_: %s\n", __PRETTY_FUNCTION__, tableName_.c_str());
}

std::string var_list:: print_cpp_var_type()
{
	stringstream var_type_str;
	switch (var_type){
		case INT32_TYPE:{
		var_type_str << "int32_t";
		}
		break;		
		case TEXT_TYPE:
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
		case NTEXT_TYPE:
		var_type_str << "std::string";
		break;		
		case FLOAT_TYPE:{
		var_type_str << "float";
		}
		break;		
		case DOUBLE_TYPE:{
		var_type_str << "double";
		}
		break;
		case BIT_TYPE:{
		var_type_str << "bool";
		}
		break;
		case DATETIME_TYPE:{
		var_type_str << "boost::gregorian::date";
		}
		break;
		case BIGINT_TYPE:
		var_type_str << "int64_t";
		break;
		case TINYINT_TYPE:
		var_type_str << "char";
		break;
		// unhandled in postgrs
		//case IMAGE_TYPE:
		//var_type_str << "Image";
		//break;
		case COMPOSITE_TYPE:
		if (options.many == true) {
			var_type_str << boost::format("std::vector<boost::shared_ptr<Biz%1%> > &")
					% var_name;	
		}
		break;

		default:
		var_type_str << "Unknown type: error";
	}
	return var_type_str.str();
}

std::string var_list::print_cpp_var_param_name()
{
	if(var_type==COMPOSITE_TYPE) {
		if (options.many==true) {
			return string("p_vec") + var_name;
		} else {
			return string("p_") + var_name;
		}
	} else {
		return string("p_") + var_name;
	}
}

std::string var_list::print_cpp_var_name()
{
	/*{
		return var_name;
	}*/
	if(var_type==COMPOSITE_TYPE) {
		if (options.many==true) {
			return string("vec") + var_name + string("_") ;
		} else {
			return var_name + string("_") ;
		}
	} else {
		return var_name + string("_") ;
	}
}

std::string var_list::print_lower_var_name()
{
	string lower_name = var_name;
	lower_name[0] = tolower(lower_name[0]);
	return lower_name;
}


std::string var_list::print_improved_lower_var_name()
{
	string orig_varname = var_name;
	int pos = orig_varname.find("_Code");
	string improved_name = orig_varname.substr(0, pos);
	improved_name[0] = tolower(improved_name[0]);
	return improved_name;
}


std::string var_list::print_sql_var_decl()
{
	stringstream var_decl_str;
	if (var_type==COMPOSITE_TYPE) {
	} else {
		var_decl_str << var_name << " " << print_sp_types(var_type);
		if (var_type==NVARCHAR_TYPE || var_type==VARCHAR_TYPE
			|| var_type==NCHAR_TYPE) {
			var_decl_str << "(" << arr_len << ")";
		}
	}
	return var_decl_str.str();
}

std::string var_list::print_sql_var_type()
{
	stringstream var_decl_str;
	if (var_type==COMPOSITE_TYPE) {
	} else {
		var_decl_str << print_sp_types(var_type);
		if (var_type==NVARCHAR_TYPE || var_type==VARCHAR_TYPE
			|| var_type==NCHAR_TYPE) {
			var_decl_str << "(" << arr_len << ")";
		}
	}
	return var_decl_str.str();
}


std::string var_list::print_sql_var_decl_for_select_return_table()
{
	stringstream var_decl_str;
	if (var_type==COMPOSITE_TYPE) {
	} else {
		var_decl_str << "r_" 
			<< var_name << " " << print_sp_types(var_type);
		if (var_type==NVARCHAR_TYPE || var_type==VARCHAR_TYPE
			|| var_type==NCHAR_TYPE) {
			var_decl_str << "(" << arr_len << ")";
		}
	}
	return var_decl_str.str();
}


std::string var_list::print_sql_var_name_for_select_return_table(string ref_table_name)
{
	stringstream var_name_str;
	if (var_type==COMPOSITE_TYPE) {
	} else {
		if( ref_table_name =="" ){
			var_name_str << "r_" 
				<< var_name ;
		} else {
			var_name_str << "r_" 
				<< ref_table_name << "_"
				<< var_name ;
		}
	}
	return var_name_str.str();
}


std::string var_list::print_random_value(int counter, int p_nRecords)
{
	static bool set_seed=true;
	if (set_seed) {
		srand(time(0));
		cout << "setting the random number seed";
		set_seed=false;
	}
	stringstream random_value_str;
	switch (var_type){
		case INT32_TYPE:{
			if(options.ref_table_name != "") {
				random_value_str << ((rand() % (p_nRecords/2)) +1);
			} else {
				random_value_str << counter;
			}
		}
		break;		
		case TEXT_TYPE:
		case NTEXT_TYPE:
			random_value_str << "'" << var_name << "_" << counter << "'";
		break;
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
			//random_value_str << "'" << var_name << "_" << counter << "'";
			//cout << __PRETTY_FUNCTION__ << ", dict.size(): " << dict.size() << endl;
			if (dict.size()==0) {
				random_value_str << "'";
				for (int i=0; i<arr_len; ++i) {
					bool break_out = false;
					char c=32;
					while (!break_out) {
						c = static_cast<char> (rand() % 128);
						if ( isalnum(c) || isblank(c) ) {
							break_out=true;
						}
					}
					random_value_str << c;
				}
				random_value_str << "'";
			} else {
				random_value_str << "'";
				int nOutput = 0;
				int nLeft =0;
				if (arr_len < 20 )
					nLeft = arr_len;
				else {
					nLeft = rand() % arr_len;
					if (nLeft<20) {
						nLeft = 20;
					}
				}
				while ( nLeft > 3) {
					bool break_out=false;
					while (!break_out) {
						int rw_index = rand() % dict.size();
						string str_without_quote;
						if (dict[rw_index].length() + 1 < nLeft) {
							break_out=true;
							int pos = dict[rw_index].find("'");
							if (pos!=string::npos) {
								str_without_quote = dict[rw_index].substr(0, pos);
							} else {
								str_without_quote = dict[rw_index];
							}
							random_value_str << str_without_quote << " ";
							nLeft -= (dict[rw_index].length()+1);
						}
					}
				}
				if (nLeft <= 3) {
					random_value_str << 'a';
				}
				random_value_str << "'";
			}
		break;
		case FLOAT_TYPE:{
		//var_type_str << "float";
			random_value_str << static_cast<float>(drand48());
		}
		break;		
		case DOUBLE_TYPE:{
			random_value_str << drand48();
		}
		break;
		case BIT_TYPE:{
			random_value_str <<  ((rand() %2) ? "'true'" : "'false'") ;
		}
		break;
		case DATETIME_TYPE:{
			//var_type_str << "boost::gregorian::date";
			int year = 2000 + (rand() % 10);
			int month = (rand() %12) + 1;
			int day;
			if(month == 2 ) {
				if (year %100 ==0) {
					if (year%400==0)
						day = rand() % 29;
					else 
						day = rand() %28;
				} else if (year % 4==0) { 
					day = rand() %29;
				} else {
					day = rand() %28;
				}
			}  else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8
					|| month == 10 || month == 12) {
				day = rand() %31;
			} else {
				day = rand() %30;
			}
			stringstream month_str;
			switch(month){
			case 1: 
				month_str << "jan";
			break;
			case 2:
				month_str << "feb";
			break;
			case 3:
				month_str << "mar";
			break;
			case 4:
				month_str << "apr";
			break;
			case 5:
				month_str << "may";
			break;
			case 6:
				month_str << "jun";
			break;
			case 7:
				month_str << "jul";
			break;
			case 8:
				month_str << "aug";
			break;
			case 9:
				month_str << "sep";
			break;
			case 10:
				month_str << "oct";
			break;
			case 11:
				month_str << "nov";
			break;
			case 12:
				month_str << "dec";
			break;
			default:
				month_str << "should-never-happen";
			}
			random_value_str << "'" << day << "-" << month_str.str() << "-" << year
				<< "'";
		}
		break;
		case BIGINT_TYPE:
		if(options.ref_table_name != "") {
			random_value_str << ((rand() % (p_nRecords/2)) +1) ;
		} else {
			random_value_str << counter;
		}
		break;
		case TINYINT_TYPE:
		if(options.ref_table_name != "") {
			random_value_str << ((rand() % (p_nRecords/2)) +1) ;
		} else {
			random_value_str << counter;
		}
		break;
		// unhandled in postgrs
		//case IMAGE_TYPE:
		//var_type_str << "Image";
		//break;
		case COMPOSITE_TYPE:
		random_value_str << ""; // ignore
		break;

		default:
		random_value_str << "Unknown type: dont know how to generate a random value";
	}
	return random_value_str.str();
}


std::string var_list::print_psql_to_cpp_conversion(std::string ref_table_name)
{
	// this first variable 
	std::stringstream s;
	using boost::format;

	//s << boost::format("\n/* %1%, %2%, %3%: ref_table_name: %4% */\n") %
	//	__FILE__ % __LINE__ % __PRETTY_FUNCTION__ % ref_table_name;

	switch (var_type){
		case INT32_TYPE:{
			s	<< "boost::lexical_cast< " << print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) )" ;
			} else {
				s << "r_" << ref_table_name << "_" << var_name << "_fnum) )";
			}
		}
		break;		
		case TEXT_TYPE:
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
		case NTEXT_TYPE: {
			s	<< "/* from here ref_table_name: " << ref_table_name << " */ "
				"boost::lexical_cast< " << print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) )" ;
			} else {
				s << "r_" << ref_table_name << "_" << var_name << "_fnum) )";
			}
		}
		break;		
		case FLOAT_TYPE: {
			s	<< "boost::lexical_cast< " << print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) )" ;
			} else {
				s << "r_" << ref_table_name << "_" << var_name << "_fnum) )";
			}
		}
		break;		
		case DOUBLE_TYPE: {
			s	<< "boost::lexical_cast< " << print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) )" ;
			} else {
				s << "r_" << ref_table_name << "_" << var_name << "_fnum) )";
			}
		}
		break;
		case BIT_TYPE: {
			s	<< "(PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) == \"t\"? true : false)" ;
			} else {
				s << "r_" << ref_table_name 
					<< "_" << var_name << "_fnum) == \"t\"? true : false)";
			}
		}
		break;
		case DATETIME_TYPE:
		{
			s	<< "boost::gregorian::from_simple_string( string( "
				<< "PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) ))" ;
			} else {
				s << "r_" << ref_table_name << "_" << var_name << "_fnum) ))";
			}
		}
		break;
		case BIGINT_TYPE: {
			s	<< "boost::lexical_cast< " << print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) )" ;
			} else {
				s << "r_" << ref_table_name << "_" << var_name << "_fnum) )";
			}
		}
		break;
		case TINYINT_TYPE: {
			s	<< "boost::lexical_cast< " << print_cpp_var_type() << " > ("
				<< "PQgetvalue(res, row, ";
			if(ref_table_name==""){
				s <<  "r_" << var_name << "_fnum ) )" ;
			} else {
				s << "r_" << ref_table_name << "_" << var_name << "_fnum) )";
			}
		}
		break;
		// unhandled in postgrs
		//case IMAGE_TYPE:
		//var_type_str << "Image";
		//break;
		case COMPOSITE_TYPE:
			s << "unhandled case: " << __FILE__ << ", " << __LINE__ << ", func: "
				<< __PRETTY_FUNCTION__ << endl;
		break;

		default:
			s << "unhandled case: " << __FILE__ << ", " << __LINE__ << ", func: "
				<< __PRETTY_FUNCTION__ << endl;
	}
	return s.str();
}

std::string TableInfoType::print_cpp_search_key_args()
{
	stringstream search_key_fields_str;
	struct var_list* v_ptr=param_list;
	if(has_search_key){
		int count=0;
		while(v_ptr){
			if(v_ptr->options.search_key){
				//search_key_fields_str <<  boost::format("\t\t");
				//search_key_fields_str << print_cpp_types(v_ptr->var_type);
				switch (v_ptr->var_type) {
				case TEXT_TYPE:
				case VARCHAR_TYPE:
				case NVARCHAR_TYPE:
				case NCHAR_TYPE:
				case NTEXT_TYPE:
					search_key_fields_str << "std::string (\"%\")";
				break;
				default:
					search_key_fields_str << boost::format(" unhandled file: %1% line: %2% func: %3%")
						% __FILE__ % __LINE__ % __PRETTY_FUNCTION__;
				}
				++count;
				if(count<has_search_key){
					search_key_fields_str <<  ",\n";
				} else 
					search_key_fields_str << "\n";
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 search_key_fields_str << "";
	}
	return search_key_fields_str.str();
}
