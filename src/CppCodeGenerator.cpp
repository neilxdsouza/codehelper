#include "std_headers.h"
#include "stmt.h"
#include "TableCollectionSingleton.hpp"
#include "CppCodeGenerator.h"
#include "utils.h"
#include "error.h"
#include "std_using.h"

using boost::format;
using boost::io::group;
extern char project_namespace[];

void CppCodeGenerator::GenerateCode(FILE * fptr)
{
	//cout << "CppCodeGenerator::GenerateCode()" << endl;
	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;

	//std::string output_code_directory_prefix = "output/CppCodeGenerator/";

	//dbCodeGenerator_->SetOutputDirectory(output_code_directory_prefix);
	//dbCodeGenerator_->SetTableInfo(tableInfo_);
	dbCodeGenerator_->GenerateCode();
	uiGenerator_->GenerateCode();
	GenerateBLL();



	cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
}

void CppCodeGenerator::GenerateBLL()
{
	//{
		cout << "outputDirPrefix_: " << outputDirPrefix_ << endl;
		string bll_h_fname (string(outputDirPrefix_.c_str()
					+ string("/")
					+ tableInfo_->tableName_
					+ string("_bll.h"))); 
		std::ofstream bll_h(bll_h_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!bll_h){
			string err_msg="unable to open " + bll_h_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		GenerateBLLDefn_h(bll_h);
	//}

	//{
		string bll_cpp_fname (string(outputDirPrefix_.c_str()
					+ string("/")
					+ tableInfo_->tableName_
					+ string("_bll.cpp"))); 
		std::ofstream bll_cpp(bll_cpp_fname.c_str(), ios_base::out|ios_base::trunc);
		if(!bll_cpp){
			string err_msg="unable to open " + bll_cpp_fname + "for writing";
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg);
		}
		GenerateBLLDefn_cpp(bll_cpp);
	//}
	bll_h << h_header.str() << h_body.str() << h_footer.str();
}

void CppCodeGenerator::GenerateBLLDefn_h(ofstream & bll_h)
{
	print_bll_h_header(bll_h);
	print_bll_api_decls(bll_h);
	print_bll_h_footer(bll_h);
}

void CppCodeGenerator::GenerateBLLDefn_cpp(ofstream & bll_cpp)
{
	bll_cpp << format("#include <boost/date_time/gregorian/gregorian.hpp>\n");
	bll_cpp << format("#include <boost/shared_ptr.hpp>\n");
	bll_cpp << format("#include <string>\n");
	bll_cpp << boost::format("#include \"%1%_bll.h\"\n") 
		% tableInfo_->tableName_;
	bll_cpp << boost::format("#include \"%1%_db_postgres.h\"\n") 
		% tableInfo_->tableName_;

	print_bll_api_defns(bll_cpp);
}

void CppCodeGenerator::print_bll_h_header(ofstream & bll_h)
{
	h_header << format("#ifndef %1%_%2%\n") 
		% project_namespace % tableInfo_->tableName_;
	h_header << format("#define %1%_%2%\n\n")
		% project_namespace % tableInfo_->tableName_;

	h_header << format("#include <boost/date_time/gregorian/gregorian_types.hpp>\n");
	h_header << format("#include <boost/shared_ptr.hpp>\n");
	h_header << endl;
}


void CppCodeGenerator::print_bll_api_decls(ofstream & bll_h)
{
	h_body << format("struct Biz%1%\n{\n") % tableInfo_->tableName_;
	print_bll_params(bll_h);
	print_bll_api_functions_decl(bll_h);
	h_body << "};\n\n";
}

void CppCodeGenerator::print_bll_h_footer(ofstream & bll_h)
{
	h_footer << format("#endif /* %1%_%2% */\n")
		% project_namespace % tableInfo_->tableName_;
}

void CppCodeGenerator::print_bll_params(std::ofstream & bll_h)
{
	struct var_list* v_ptr=tableInfo_->param_list;
	stringstream variables, functions;
	stringstream var_type;
	//variables << "private:\n";

	bool emitted_vector_header = false;
	bool emitted_string_header = false;
	while(v_ptr){
		if (v_ptr->options.ref_table_name!="") {
			if (v_ptr->options.many==false) {
				var_type << boost::format("boost::shared_ptr<Biz%1%>") % v_ptr->options.ref_table_name;
			} else {
				var_type << boost::format("std::vector<boost::shared_ptr<Biz%1%> >") 
					% v_ptr->options.ref_table_name;
			}
			if (!emitted_vector_header) {
				h_header << boost::format("#include <vector>\n");
				emitted_vector_header = true;
			}
			//h_header << boost::format("#include \"%1%_bll.h\" /*1*/\n") %
			//		v_ptr->options.ref_table_name;
		} else {
			var_type << print_cpp_types(v_ptr->var_type);
			if (!emitted_string_header) {
				switch (v_ptr->var_type) {
				case TEXT_TYPE:
				case VARCHAR_TYPE:
				case NVARCHAR_TYPE:
				case NCHAR_TYPE:
				case NTEXT_TYPE:
					h_header << boost::format("#include <string>\n");
					emitted_string_header = true;
				break;
				default:
					;
				}
			}
			/*
			switch (v_ptr->var_type){
				case INT32_TYPE:{
				var_type << "int32_t";
				}
				break;		
				case TEXT_TYPE:
				case VARCHAR_TYPE:
				case NVARCHAR_TYPE:
				case NCHAR_TYPE:
				case NTEXT_TYPE:
				var_type << "std::string";
				if(!emitted_string_header){
					h_header << boost::format("#include <string>\n");
					emitted_string_header = true;
				}
				break;		
				case FLOAT_TYPE:{
				var_type << "float";
				}
				break;		
				case DOUBLE_TYPE:{
				var_type << "double";
				}
				break;
				case BIT_TYPE:{
				var_type << "bool";
				}
				break;
				case DATETIME_TYPE:{
				var_type << "boost::gregorian::date";
				}
				break;
				case BIGINT_TYPE:
				var_type << "int64_t";
				break;
				case TINYINT_TYPE:
				var_type << "byte";
				break;
				case IMAGE_TYPE:
				var_type << "Image";
				break;

				default:
				var_type << "Unknown type: error";

			}
			*/
		}

		if (v_ptr->options.ref_table_name!="") {
			if ( ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
				//variables << "/* --from here : " << __FILE__ << ", " << __LINE__ 
				//	<< ", " << __PRETTY_FUNCTION__ << "*/ "
				//	<< endl;
				variables << v_ptr->print_cpp_var_type() << boost::format(" %1%_;\n") % v_ptr->var_name;
				functions << "\t";
				functions << boost::format("%2% Get_%1%() { return %1%_;}\n") 
					% v_ptr->var_name % v_ptr->print_cpp_var_type();
				functions << "\t";
				functions << boost::format("void Set_%1%(%2%  value) { %1%_= value;}\n") 
					% v_ptr->var_name % v_ptr->print_cpp_var_type();
				//variables.str("");
				//functions.str("");
				//var_type.str("");
				//variables.clear();
				//functions.clear();
				//var_type.clear();
				//v_ptr=v_ptr->prev;
				//continue;
			} else if (v_ptr->options.many==true) {
				h_header << boost::format("#include \"%1%_bll.h\" /*1*/\n") %
						v_ptr->options.ref_table_name;
				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				variables << var_type.str() << boost::format(" biz_vec_%1%_;\n") % improved_name;
				functions << "\t";
				functions << boost::format("%2% & Get_%1%() { return biz_vec_%1%_;}\n") 
					% improved_name % var_type.str() ;
				functions << "\t";
				functions << boost::format("void Set_%1%(%2% & value) { biz_vec_%1%_= value;}\n") 
					% improved_name % var_type.str();
			} else {
				h_header << boost::format("#include \"%1%_bll.h\" /*1*/\n") %
						v_ptr->options.ref_table_name;
				variables << var_type.str() << boost::format(" biz_%1%_;\n") % 
					v_ptr->print_improved_lower_var_name();

				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);

				functions << boost::format("\t%1% Get_%2%()\n\t{\n\t\tif(biz_%3%_)\n\t\t\treturn biz_%3%_->%2%_;\n\t\telse return -1;\n\t}\n") 
					% v_ptr->print_cpp_var_type()
					% v_ptr->var_name 
					% v_ptr->print_improved_lower_var_name()
					;
				functions << boost::format("\tvoid Set_%2%(%1%  value ) { biz_%3%_->%2%_ = value; }\n") 
					% v_ptr->print_cpp_var_type()
					% v_ptr->var_name 
					% v_ptr->print_improved_lower_var_name()
					;

				functions << boost::format("\tboost::shared_ptr<Biz%1%> & Get_%1%()\n\t{\n\t\t\treturn biz_%2%_;\n\t}\n") 
					% improved_name
					% v_ptr->print_improved_lower_var_name()
					;
				functions << boost::format("\tvoid Set_%1%(boost::shared_ptr<Biz%1%>  value ) { biz_%2%_ = value; }\n") 
					% improved_name
					% v_ptr->print_improved_lower_var_name()
					;

			}
		} else {
			variables << var_type.str() << boost::format(" %1%_;\n") % v_ptr->var_name;
			functions << "\t";
			functions << boost::format("%2% Get_%1%() { return %1%_;}\n") 
				% v_ptr->var_name % var_type.str();
			functions << "\t";
			functions << boost::format("void Set_%1%(%2%  value) { %1%_= value;}\n") 
				% v_ptr->var_name % var_type.str();
		}

		h_body << functions.str();
		h_body << "\t" << variables.str();
		/*
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			h_body << "\t\tpublic ";
			print_csharp_types(h_body, v_ptr->var_type);
			h_body << v_ptr->var_name;
		}
		*/

		v_ptr=v_ptr->prev;
		variables.str("");
		functions.str("");
		var_type.str("");
		variables.clear();
		functions.clear();
		var_type.clear();
	}
	// v_ptr=tableInfo_->param_list;
	// print the properties of Fields which are inside a composite object
	// I need to modify the input language to accept UI flags but for now 
	// hard code everything

	/* Dont need this for c++
	   */
	/*
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(stderr, "%s %s : %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
			struct CppCodeGenerator * t_ptr = (dynamic_cast<CppCodeGenerator*>
					(TableCollectionSingleton::Instance()
					 	.my_find_table(v_ptr->options.ref_table_name)));
			struct var_list* vv_ptr = t_ptr->tableInfo_->param_list;
			while(vv_ptr){
				h_body << "\t\tpublic ";
				print_csharp_types(h_body, vv_ptr->var_type);
				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos) ;
				//fprintf(fptr, " %s%s{\n", v_ptr->var_name.c_str(), vv_ptr->var_name.c_str());
				h_body << boost::format(" %s_%s{\n") % improved_name % vv_ptr->var_name;
				h_body << "\t\t\tget{\n";
				h_body << boost::format("\t\t\t\tif (%1% !=null){\n") % improved_name;
				h_body << boost::format("\t\t\t\t\treturn %1%.%2%;\n") % improved_name
					% vv_ptr->var_name;
				h_body << "\t\t\t\t} else \n";
				h_body << "\t\t\t\t\treturn null;\n";
				h_body << "\t\t\t\t}\n";
				h_body << "\t\t\t}\n";
				vv_ptr=vv_ptr->prev;
			}
		}
		v_ptr=v_ptr->prev;
	}
	*/
}

void CppCodeGenerator::print_bll_api_functions_decl(std::ofstream & bll_h)
{
	h_body << "\t//bool ValidateForInsert();\n";
	h_body << "\tint Insert();\n";
	h_body << "\t//bool ValidateForUpdate();\n";
	h_body << "\t//int Update();\n";
	print_bll_api_constructors();
}

void CppCodeGenerator::print_bll_Insert_defn(std::ofstream & bll_cpp)
{
	cpp_body << boost::format("int Biz%1%::Insert()\n")
		% tableInfo_->tableName_;
	cpp_body << "{\n";
	cpp_body << boost::format("\tint ret=%1%::db::%2%::Insert%2%(*this);\n")
			% project_namespace % tableInfo_->tableName_;
	cpp_body << "}\n";
}

void CppCodeGenerator::print_bll_api_defns(std::ofstream & bll_cpp)
{
	print_bll_Insert_defn(bll_cpp);
	print_bll_api_test_stubs(bll_cpp);
	print_bll_Constructor_defn(bll_cpp);
	bll_cpp << cpp_body.str();
}

void CppCodeGenerator::print_bll_api_test_stubs(std::ofstream & bll_cpp)
{
	cpp_body << "/* \nint main()\n";
	cpp_body << "{\n";
	cpp_body << boost::format("\tBiz%1% test_%1%;\n")
			% tableInfo_->tableName_;
	for(int i=0; i<tableInfo_->vec_var_list.size(); ++i){
		if (tableInfo_->vec_var_list[i]->var_type==COMPOSITE_TYPE) {
			continue;
		}
		cpp_body << boost::format("\ttest_%1%.Set_%2%(")
				% tableInfo_->tableName_ 
				% tableInfo_->vec_var_list[i]->var_name;
		switch(tableInfo_->vec_var_list[i]->var_type){
		case IMAGE_TYPE:
			cpp_body << "ERROR: dummy default value not implemented IMAGE_TYPE";
			break;
		case BIGINT_TYPE:
			cpp_body << "12345678910111213";
			break;
		case TINYINT_TYPE:
			cpp_body << "128";
			break;
		case INT32_TYPE:
			cpp_body << "1234567";
			break;
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
		case NTEXT_TYPE:
		case TEXT_TYPE:
			cpp_body << "std::string(\"test_string\")";
			break;
		case BIT_TYPE:
			cpp_body << "true";
			break;
		case DATETIME_TYPE:
			cpp_body << "boost::gregorian::date(boost::gregorian::from_simple_string(\"2001-10-14\"))";
			break;
		default:
			cpp_body << "unknown type:\n";
			fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, 
				string("Fix me : exit_nicely may not be requires since the PGconn has a custom deleter which closes the connection\n"));
		}
		cpp_body << ");\n";
	}
	cpp_body << boost::format("\ttest_%1%.Insert();\n")
		% tableInfo_->tableName_;
	cpp_body << "}\n*/\n";
}

void CppCodeGenerator::FinalCleanUp()
{
	uiGenerator_->FinalCleanUp();
}


void CppCodeGenerator::print_bll_api_constructors()
{
	print_bll_constructor_decl_with_all_fields();
	if (tableInfo_->nInvisible>0)
		print_bll_constructor_decl_without_invisible_fields();
	h_body << print_bll_empty_constructor_decl();

	if (tableInfo_->has_composite_objs) {
		struct var_list* v_ptr=tableInfo_->param_list;
		int count=0;
		while (v_ptr) {
			if (v_ptr->options.ref_table_name != "" && v_ptr->options.many==false) {
				if (ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name)) {
				} else {
					++count;
				}
			}
			v_ptr = v_ptr->prev;
		}
		if (count >0 ) {
			h_body << "/* count : " << count << " */\n";
			h_body << print_bll_bizobj_constructor_h();
		}
	}
}

void CppCodeGenerator::print_bll_constructor_decl_with_all_fields()
{
	h_body << "\n";
	h_body << boost::format("\tBiz%1%(\n")
			% tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	bool output_comma = false;
	while (v_ptr) {
		if (v_ptr->options.ref_table_name != "" 
			&& v_ptr->options.many==false) {
			if (ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name)) {
				h_body << format("\t\t/* skip: line: %1%, file: %2%, func: %3%*/\n") %
					__LINE__ % __FILE__ % __PRETTY_FUNCTION__;
				h_body << "\t\t" << v_ptr->print_cpp_var_type()
					<< " " << v_ptr->print_cpp_var_param_name();
				output_comma = true;
			} else {
				h_body << format("\t\t/* dont skip: line: %1%, file: %2%, func: %3%*/\n") %
					__LINE__ % __FILE__ % __PRETTY_FUNCTION__;
				h_body << "\t\t" << v_ptr->print_cpp_var_type()
					<< " " << v_ptr->print_cpp_var_param_name();
				output_comma = true;
			}
		} else {
			h_body << "\t\t" << v_ptr->print_cpp_var_type()
				<< " " << v_ptr->print_cpp_var_param_name();
			output_comma = true;
		}
		v_ptr=v_ptr->prev;		
		if(v_ptr && output_comma==true){
			h_body << ",\n";
			output_comma = false;
		}
	}
	h_body << "\t);\n";
}


void CppCodeGenerator::print_bll_constructor_decl_without_invisible_fields()
{
	h_body << "\n";
	h_body << boost::format("\tBiz%1%(\n")
			% tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	/*
	while (v_ptr && v_ptr->visible) {
		h_body << "\t\t" << v_ptr->print_cpp_var_type()
			<< " " << v_ptr->print_cpp_var_param_name();
		v_ptr=v_ptr->prev;		
		if(v_ptr){
			h_body << ",\n";
		}
	}
	h_body << "\t);\n";
	*/
	// ==============================
	while(v_ptr) {
		if ( v_ptr->options.visible==true ) {
			h_body << "\t\t" << v_ptr->print_cpp_var_type()
				<< " " << v_ptr->print_cpp_var_param_name();
			if (v_ptr->prev && v_ptr->prev->options.visible==true ) {
				h_body << ",\n";
			}
		}
		v_ptr=v_ptr->prev;		
	}
	h_body << "\t);\n";
}

// At this point I realise that this is wrong
// print_bll_bizobj_constructor_cpp and print_bll_bizobj_constructor_h 
// should both be invoked from the same place -
// which means I need to do some serious code refactoring
void CppCodeGenerator::print_bll_Constructor_defn(std::ofstream & bll_cpp)
{
	print_bll_Constructor_with_all_fields();
	if (tableInfo_->nInvisible>0)
		print_bll_Constructor_without_invisible_fields();
	cpp_body << print_bll_empty_constructor_defn();
	if (tableInfo_->has_composite_objs) {
		struct var_list* v_ptr=tableInfo_->param_list;
		int count=0;
		while (v_ptr) {
			if (v_ptr->options.ref_table_name != "" && v_ptr->options.many==false) {
				if (ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name)) {
				} else {
					++count;
				}
			}
			v_ptr = v_ptr->prev;
		}
		if (count >0 ) {
			cpp_body << print_bll_bizobj_constructor_cpp();
		}
	}
}

#if 0
void CppCodeGenerator::print_bll_Constructor_with_all_fields()
{
	print_bll_Constructor_with_all_fields();
	print_bll_Constructor_without_invisible_fields();
}
#endif /* 0 */

void CppCodeGenerator::print_bll_Constructor_with_all_fields()
{
	cpp_body << "\n";
	cpp_body << boost::format("Biz%1%::Biz%1%(\n")
			% tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		cpp_body << "\t\t" << v_ptr->print_cpp_var_type()
			<< " " << v_ptr->print_cpp_var_param_name();
		v_ptr=v_ptr->prev;		
		if(v_ptr){
			cpp_body << ",\n";
		}
	}
	cpp_body << "\t)\n\t:\n";
	v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		bool skip_comma = false;
		if (v_ptr->options.ref_table_name == "" ||
			ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
			cpp_body <<  "\t" << v_ptr->print_cpp_var_name() 
				<< "("
				<< v_ptr->print_cpp_var_param_name()
				<< ")"; 
		} else {
			skip_comma=true;
		}
		v_ptr=v_ptr->prev;
		if(!skip_comma) {
			if(v_ptr){
				cpp_body << ",\n";
			}
		}
	}

	cpp_body << "\n{\n";
	v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if ( v_ptr->options.ref_table_name != "" && v_ptr->options.many == false
			&&	
			(!ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) )) {
			cpp_body << boost::format("\tbiz_%1%_->%2%_ = %3%;\n") 
				% v_ptr->print_improved_lower_var_name()
				% v_ptr->var_name 
				% v_ptr->print_cpp_var_param_name()
				;
		}
		v_ptr=v_ptr->prev;		
	}
	cpp_body << "}\n";
}

void CppCodeGenerator::print_bll_Constructor_without_invisible_fields()
{
	cpp_body << "\n";
	cpp_body << boost::format("Biz%1%::Biz%1%(\n")
			% tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if ( v_ptr->options.visible==true ) {
			cpp_body << "\t\t" << v_ptr->print_cpp_var_type()
				<< " " << v_ptr->print_cpp_var_param_name();
			if (v_ptr->prev && v_ptr->prev->options.visible==true ) {
				cpp_body << ",\n";
			}
		}
		v_ptr=v_ptr->prev;		
	}
	cpp_body << "\t)\n\t:\n";
	v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if ( v_ptr->options.visible==true ) {
			cpp_body <<  "\t" << v_ptr->print_cpp_var_name() 
				<< "("
				<< v_ptr->print_cpp_var_param_name()
				<< ")"; 
			if (v_ptr->prev && v_ptr->prev->options.visible==true ) {
				cpp_body << ",\n";
			}
		}
		v_ptr=v_ptr->prev;		
	}
	cpp_body << "\n{ }\n";
}

/*
bool ReferencedTableContainsUs(TableInfoType* me, std::string ref_table_name)
{
	TableInfoType * ti_ptr = find_TableInfo(ref_table_name);
	if (ti_ptr ==0 ){
		cerr << " Referenced table: " << ref_table_name << " not found ... exiting";
		exit(1);
	}
	
	struct var_list* v_ptr=ti_ptr->param_list;
	while (v_ptr) {
		if (v_ptr->var_name == me->tableName_ &&
				v_ptr->var_type==COMPOSITE_TYPE) {
			
			return true;
		}
		v_ptr=v_ptr->prev;
	}
	return false;
}
*/


std::string CppCodeGenerator::print_bll_empty_constructor_decl()
{
	stringstream str;
	str << boost::format("\tBiz%1%();\n")
			% tableInfo_->tableName_;
	return str.str();
}

std::string CppCodeGenerator::print_bll_empty_constructor_defn()
{
	stringstream str;
	str << boost::format("\nBiz%1%::Biz%1%()\n{ }\n\n")
			% tableInfo_->tableName_;
	return str.str();
}


std::string CppCodeGenerator::print_bll_bizobj_constructor_h()
{
	using boost::format;
	stringstream bll_bizobj_constructor_h;
	bll_bizobj_constructor_h << format("\tBiz%1%(")
			% tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		bll_bizobj_constructor_h << "\n\t\t";
		if(v_ptr->options.ref_table_name==""){
			bll_bizobj_constructor_h << print_cpp_types(v_ptr->var_type);
			bll_bizobj_constructor_h << format(" l_%1%")
				% v_ptr->var_name;
		} else {
			string orig_varname = v_ptr->var_name;
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			if(v_ptr->options.many){
				bll_bizobj_constructor_h << 
					format ("std::vector<boost::shared_ptr<Biz%1%> > l_biz_%2%") 
						% v_ptr->options.ref_table_name
						% improved_name;
			} else {
				bll_bizobj_constructor_h 
					<< format ("boost::shared_ptr<Biz%1%> l_biz_%2%")
						% v_ptr->options.ref_table_name
						% improved_name;
			}
		}
		v_ptr=v_ptr->prev;
		if(v_ptr){
			bll_bizobj_constructor_h <<  ", ";
		}
	}
	bll_bizobj_constructor_h << ");\n";

	/*
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!=""){
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			fprintf(fptr, "\t\tthis.%s= l_biz_%s;\n", improved_name.c_str(), improved_name.c_str());
		} else {
			fprintf(fptr, "\t\tthis.%s= l_%s;\n", v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "\t}\n");
	*/
	return bll_bizobj_constructor_h.str();
}


std::string CppCodeGenerator::print_bll_bizobj_constructor_cpp()
{
	using boost::format;
	stringstream bizobj_constructor_body;
	bizobj_constructor_body << format("Biz%1%::Biz%1%(")
			% tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		bizobj_constructor_body << "\n\t\t";
		if(v_ptr->options.ref_table_name==""){
			bizobj_constructor_body << print_cpp_types(v_ptr->var_type);
			bizobj_constructor_body << format(" l_%1%")
				% v_ptr->var_name;
		} else {
			string orig_varname = v_ptr->var_name;
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			if(v_ptr->options.many){
				bizobj_constructor_body << 
					format ("std::vector<boost::shared_ptr<Biz%1%> > l_biz_vec_%2%") 
						% v_ptr->options.ref_table_name
						% improved_name;
			} else {
				bizobj_constructor_body 
					<< format ("boost::shared_ptr<Biz%1%> l_biz_%2%")
						% v_ptr->options.ref_table_name
						% improved_name;
			}
		}
		v_ptr=v_ptr->prev;
		if(v_ptr){
			bizobj_constructor_body <<  ", ";
		}
	}
	bizobj_constructor_body << ")\n\t:\n";

	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!=""){
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			if (v_ptr->options.many) {
				bizobj_constructor_body << format ("\t\tbiz_vec_%1%_ ( l_biz_vec_%1%)") % improved_name;
			} else {
				bizobj_constructor_body << format ("\t\tbiz_%1%_ ( l_biz_%2%)") % v_ptr->print_improved_lower_var_name()
						% improved_name;
			}
		} else {
			bizobj_constructor_body << format("\t\t%1%_(l_%1%)") % v_ptr->var_name;
		}
		v_ptr=v_ptr->prev;
		if ( v_ptr ) {
			bizobj_constructor_body << format(",\n");
		}
	}
	bizobj_constructor_body << format ("\n{ }\n");
	return bizobj_constructor_body.str();
}
