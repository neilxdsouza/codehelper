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
	{
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
	}

	{
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
	}
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
}

void CppCodeGenerator::print_bll_h_header(ofstream & bll_h)
{
	bll_h << format("#ifndef %1%_%2%\n") 
		% project_namespace % tableInfo_->tableName_;
	bll_h << format("#define %1%_%2%\n\n")
		% project_namespace % tableInfo_->tableName_;

	bll_h << format("#include <boost/date_time/gregorian/gregorian_types.hpp>\n");
	bll_h << format("#include <boost/shared_ptr.hpp>\n");
	bll_h << endl;
}


void CppCodeGenerator::print_bll_api_decls(ofstream & bll_h)
{
	bll_h << format("struct %1%\n{\n") % tableInfo_->tableName_;
	print_bll_params(bll_h);
	bll_h << "};\n\n";
}

void CppCodeGenerator::print_bll_h_footer(ofstream & bll_h)
{
	bll_h << format("#endif /* %1%_%2% */\n")
		% project_namespace % tableInfo_->tableName_;
}

void CppCodeGenerator::print_bll_params(std::ofstream & bll_h)
{
	struct var_list* v_ptr=tableInfo_->param_list;
#define LARGE_BUFF 1024
	//char buff1[LARGE_BUFF];
	//char buff2[LARGE_BUFF];
	//int nwrite1=0, nwrite2=0;
	//variables << "private:\n";
	stringstream variables, functions;
	stringstream var_type;
	variables << "public:\n";
	functions << "public:\n";

	while(v_ptr){
		//char * bufptr1=buff1, *bufptr2=buff2;
		//nwrite1=sprintf(bufptr1, "\t\tprivate ");
		//nwrite2=sprintf(bufptr2, "\t\tpublic ");
		//bufptr1 += nwrite1;
		//bufptr2 += nwrite2;
		if(v_ptr->options.ref_table_name!=""){
			if(v_ptr->options.many==false){
				//nwrite1=sprintf(bufptr1, "Biz%s", v_ptr->options.ref_table_name.c_str());
				//nwrite2=sprintf(bufptr2, "Biz%s", v_ptr->options.ref_table_name.c_str());
				var_type << boost::format("Biz%1%") % v_ptr->options.ref_table_name;
			} else {
				//nwrite1=sprintf(bufptr1, "vector<boost::shared_ptr<Biz%s> >", v_ptr->options.ref_table_name.c_str());
				//nwrite2=sprintf(bufptr2, "vector<boost::shared_ptr<Biz%s> >", v_ptr->options.ref_table_name.c_str());
				var_type << boost::format("vector<boost::shared_ptr<Biz%1%> >") 
					%v_ptr->options.ref_table_name;
			}

		} else {
			switch (v_ptr->var_type){
				case INT32_TYPE:{
				//nwrite1=sprintf(bufptr1, "int");
				//nwrite2=sprintf(bufptr2, "int");
				var_type << "int32_t";

				}
				break;		
				case TEXT_TYPE:
				case VARCHAR_TYPE:
				case NVARCHAR_TYPE:
				case NCHAR_TYPE:
				case NTEXT_TYPE:
				//nwrite1=sprintf(bufptr1, "string");
				//nwrite2=sprintf(bufptr2, "string");
				var_type << "string";
				
				break;		
				case FLOAT_TYPE:{
				//nwrite1=sprintf(bufptr1, "float");
				//nwrite2=sprintf(bufptr2, "float");
				var_type << "float";

				}
				break;		
				case DOUBLE_TYPE:{
				//nwrite1=sprintf(bufptr1, "double");
				//nwrite2=sprintf(bufptr2, "double");
				var_type << "double";

				}
				break;
				case BIT_TYPE:{
				//nwrite1=sprintf(bufptr1, "bool");
				//nwrite2=sprintf(bufptr2, "bool");
				var_type << "bool";

				}
				break;
				case DATETIME_TYPE:{
				//nwrite1=sprintf(bufptr1, "date");
				//nwrite2=sprintf(bufptr2, "date");
				var_type << "date";

				}
				break;
				case BIGINT_TYPE:
				//nwrite1=sprintf(bufptr1, "Int64");
				//nwrite2=sprintf(bufptr2, "Int64");
				var_type << "int64_t";
				break;
				case TINYINT_TYPE:
				//fprintf(fptr, "byte");
				//nwrite1=sprintf(bufptr1, "byte");
				//nwrite2=sprintf(bufptr2, "byte");
				var_type << "byte";
				break;
				case IMAGE_TYPE:
				//fprintf(fptr, "Image");
				//nwrite1=sprintf(bufptr1, "Image");
				//nwrite2=sprintf(bufptr2, "Image");
				var_type << "Image";
				break;

				default:
				//nwrite1=sprintf(bufptr1, "Unknown type : error ");
				//nwrite2=sprintf(bufptr2, "Unknown type : error ");
				var_type << "Unknown type: error";

			}
		}
		//bufptr1 += nwrite1;
		//bufptr2 += nwrite2;
			//nwrite1=sprintf(bufptr1, " <_%s>;\n", v_ptr->var_name.c_str());
		if(v_ptr->var_type==DATETIME_TYPE){
			//nwrite1=sprintf(bufptr1, " %s_;\n", v_ptr->var_name.c_str());
			variables << var_type.str() << format(" %1%_;") % v_ptr->var_name << "// came here\n";
			//nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
			//		v_ptr->var_name.c_str(), 
			//		v_ptr->var_name.c_str(), 
			//		v_ptr->var_name.c_str());
		} else {
			if(v_ptr->options.ref_table_name!=""){
				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				//nwrite1=sprintf(bufptr1, " %s_;\n", improved_name.c_str());
				variables << boost::format("%1%_;\n") % improved_name;
				//nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
				//		improved_name.c_str(), 
				//		improved_name.c_str(), 
				//		improved_name.c_str());
				functions << boost::format("\t%2% Get_%1%() { return %1%_;}\n") 
					% improved_name % var_type.str() ;
				functions << boost::format("\tvoid Set_%1%(%2% value) { %1%_= value;}\n") 
					% improved_name % var_type.str();
			} else {
				//nwrite1=sprintf(bufptr1, " _%s;\n", v_ptr->var_name.c_str());
				variables << var_type.str() << boost::format(" %1%_;\n") % v_ptr->var_name;
				//nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
				//		v_ptr->var_name.c_str(), 
				//		v_ptr->var_name.c_str(), 
				//		v_ptr->var_name.c_str());
				functions << var_type.str() << boost::format(" Get_%1%() { return %1%_;}\n") % v_ptr->var_name;
				functions << boost::format("\tvoid Set_%1%(%2% value) { %1%_= value;}\n") 
					% v_ptr->var_name % var_type.str();
			}
		}
		//nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
		//		v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		///fprintf(fptr, "%s", buff1);
		///fprintf(fptr, "%s", buff2);
		//bll_h << buff1;
		//bll_h << buff2;
		bll_h << "\t" << functions.str();
		bll_h << "\t" << variables.str();
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			//fprintf(fptr, "\t\tpublic ");
			bll_h << "\t\tpublic ";
			print_csharp_types(bll_h, v_ptr->var_type);
			//fprintf(fptr, " %s;\n", v_ptr->var_name.c_str());
			bll_h << v_ptr->var_name;
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
			//struct CSharpAspNetCodeGenerator* t_ptr = my_find_table( v_ptr->options.ref_table_name);
			//struct CSharpAspNetCodeGenerator* t_ptr = ptrCreateTableStatementArray->my_find_table(v_ptr->options.ref_table_name);
			fprintf(stderr, "%s %s : %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
			//struct CSharpAspNetCodeGenerator* t_ptr = TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance().my_find_table(v_ptr->options.ref_table_name);
			struct CppCodeGenerator * t_ptr = (dynamic_cast<CppCodeGenerator*>
					(TableCollectionSingleton::Instance()
					 	.my_find_table(v_ptr->options.ref_table_name)));
			struct var_list* vv_ptr = t_ptr->tableInfo_->param_list;
			while(vv_ptr){
				bll_h << "\t\tpublic ";
				print_csharp_types(bll_h, vv_ptr->var_type);
				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos) ;
				//fprintf(fptr, " %s%s{\n", v_ptr->var_name.c_str(), vv_ptr->var_name.c_str());
				bll_h << boost::format(" %s_%s{\n") % improved_name % vv_ptr->var_name;
				bll_h << "\t\t\tget{\n";
				bll_h << boost::format("\t\t\t\tif (%1% !=null){\n") % improved_name;
				bll_h << boost::format("\t\t\t\t\treturn %1%.%1%;\n") % improved_name
					% vv_ptr->var_name;
				bll_h << "\t\t\t\t} else \n";
				bll_h << "\t\t\t\t\treturn null;\n";
				bll_h << "\t\t\t\t}\n";
				bll_h << "\t\t\t}\n";
				vv_ptr=vv_ptr->prev;
			}
		}
		v_ptr=v_ptr->prev;
	}
}

void CppCodeGenerator::print_bll_api(std::ofstream & bll_cpp)
{
}
