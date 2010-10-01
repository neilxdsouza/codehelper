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
	bll_cpp << boost::format("#include \"%1%_bll.h\"\n") 
		% tableInfo_->tableName_;
	bll_cpp << boost::format("#include \"%1%_db_postgres.h\"\n") 
		% tableInfo_->tableName_;
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
		if(v_ptr->options.ref_table_name!=""){
			if(v_ptr->options.many==false){
				var_type << boost::format("Biz%1%") % v_ptr->options.ref_table_name;
			} else {
				var_type << boost::format("std::vector<boost::shared_ptr<Biz%1%> >") 
					% v_ptr->options.ref_table_name;
			}
			if(!emitted_vector_header){
				h_header << boost::format("#include <vector>\n");
				emitted_vector_header = true;
			}
			h_header << boost::format("#include \"%1%_bll.h\"\n") %
					v_ptr->options.ref_table_name;
		} else {
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
		}

		if(v_ptr->options.ref_table_name!=""){
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			variables << var_type.str() << boost::format(" vec%1%_;\n") % improved_name;
			functions << "\t";
			functions << boost::format("%2% & Get_%1%() { return vec%1%_;}\n") 
				% improved_name % var_type.str() ;
			functions << "\t";
			functions << boost::format("void Set_%1%(%2% & value) { vec%1%_= value;}\n") 
				% improved_name % var_type.str();
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
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			h_body << "\t\tpublic ";
			print_csharp_types(h_body, v_ptr->var_type);
			h_body << v_ptr->var_name;
		}

		v_ptr=v_ptr->prev;
		variables.str("");
		functions.str("");
		var_type.str("");
		variables.clear();
		functions.clear();
		var_type.clear();
	}
	v_ptr=tableInfo_->param_list;
	// print the properties of Fields which are inside a composite object
	// I need to modify the input language to accept UI flags but for now 
	// hard code everything
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
}

void CppCodeGenerator::print_bll_api_functions_decl(std::ofstream & bll_h)
{
	h_body << "\t//bool ValidateForInsert();\n";
	h_body << "\tint Insert();\n";
	h_body << "\t//bool ValidateForUpdate();\n";
	h_body << "\t//int Update();\n";
}

void CppCodeGenerator::print_bll_Insert_defn()
{
}
