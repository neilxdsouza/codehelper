/*
 *  tree.C implementation of tree structures for parsed grammar
 *  Copyright (C) 2008, 2009 Neil Xavier D'Souza
 *  Released under GNU GPL Version 2
 */

#include "tree.h"
#include "utils.h"


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


std::string var_list::print_sql_var_name_for_select_return_table()
{
	stringstream var_name_str;
	if (var_type==COMPOSITE_TYPE) {
	} else {
		var_name_str << "r_" 
			<< var_name ;
	}
	return var_name_str.str();
}
