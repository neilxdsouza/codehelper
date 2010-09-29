#include "std_headers.h"
#include <cstdio>
#include <cstdlib>
#include <boost/program_options.hpp>


#include "CSharpAspNetCodeGenerator.h"
#include "CSharpAspNetCodeGeneratorFactory.h"

#include "CppCodeGenerator.h"
#include "CppCodeGeneratorFactory.h"

#include "PostgreSQLCodeGeneratorFactory.h"
#include "PostgreSQLCodeGenerator.h"
#include "TableCollectionSingleton.hpp"
#include "stmt.h"
#include "ForwardDecl.h"
#include "std_using.h"

//AbstractCodeGenerator.h
//ForwardDecl.h
//integrated_sql_gram.h
//my_defs.h
//scope.h
//std_headers.h
//std_using.h
//stmt.h
//symtab.h
//tree.h
//utils.h


void yyrestart (FILE *);
int yyparse();
extern int no_errors;

#define MAX_NAMESPACE_WORD 1023
char project_namespace[MAX_NAMESPACE_WORD]={"TopLevel_Namespace"};
string rhs_name_space_name;
void print_code(FILE * &edit_out);
AbstractCodeGeneratorFactory * codeGeneratorFactory;

void Init();

	vector <scope*> active_scope_list;
	scope* active_scope;
	struct stmt * tree_root=0;

void ParseProgramOptions(int argc, char* argv[]);

int main(int argc, char* argv[], char* envp[])
{

	ParseProgramOptions(argc, argv);
	if(argc!=3) {
		cout << "Usage: " << argv[0] << "  <inp-file> <namespace_name>" << endl;
		exit(0);
	}
	Init();
	rhs_name_space_name=argv[2];
	std::string output_code_directory_prefix = "output/CppCodeGenerator/";
	//PostgreSQLCodeGenerator psqlCodeGenerator;
	PostgreSQLCodeGeneratorFactory psqlFactory; 
	CSharpAspnetCodeGeneratorFactory
		cSharpAspNetCodeGeneratorFactory(&psqlFactory,
						 output_code_directory_prefix);
	CppCodeGeneratorFactory
		cppCodeGeneratorFactory(&psqlFactory,
					output_code_directory_prefix);
	//codeGeneratorFactory = &cSharpAspNetCodeGeneratorFactory;
	codeGeneratorFactory = &cppCodeGeneratorFactory;
	
	
	FILE * yyin=fopen(argv[1],"r");
	yyrestart(yyin);
	if(yyparse()){
		cout << "Errors in parsing: " << no_errors << endl;
		exit(1);
	} else
		cout << "yyparse finished : now going to print tree: no_errors: "
			<< " should be 0 or we have a bug in the compiler"<< endl;
	if(!no_errors){
		FILE * edit_out= fopen("edit_out.c", "wb");
		if(edit_out==NULL){
			printf("could not open edit_out.c for writing\n");
			exit(1);
		}
		//tree_root->GenerateCode(edit_out);
		print_code(edit_out);
		fclose(edit_out);
	} else {
		cerr << "Errors in Parse:  Total errors: " << no_errors << endl;
	}
	return no_errors;
}

/*
int lookup_func(string table_name_index)
{
	for(register unsigned int i=0; i<table_info_table.size(); ++i){
		if(table_name_index==table_info_table[i]->tableName_){
			return i;
		}
	}
	return -1;
}
*/

void print_code(FILE * & edit_out)
{
	printf("ENTER print_code\n");
	tree_root->GenerateCode(edit_out);
}

void Init()
{
	//CreateTableStatementArray = TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance();
	//ptrCreateTableStatementArray = TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance();
	//TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance();
	TableCollectionSingleton::Instance();
	active_scope=new scope();
	active_scope_list.push_back(active_scope);
}


void ParseProgramOptions(int argc, char * argv[])
{
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce this help message")
		("top-level-namespace", "Top level namespace of the project - for example MyProject");
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
}
