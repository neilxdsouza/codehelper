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

#include "WtUIGeneratorFactory.h"
#include "WtUIGenerator.h"


#include "TableCollectionSingleton.hpp"
#include "stmt.h"
#include "ForwardDecl.h"
#include "std_using.h"
#include "global_options.h"
#include "global_variables.h"

#include "simple_graph.h"

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
//char project_namespace[MAX_NAMESPACE_WORD]={"TopLevel_Namespace"};
string rhs_name_space_name;
void print_code(FILE * &edit_out);
AbstractCodeGeneratorFactory * codeGeneratorFactory;
vector<string> dict;

void Init();

	vector <scope*> active_scope_list;
	scope* active_scope;
	struct stmt * tree_root=0;

void ParseProgramOptions(int argc, char* argv[]);
//std::string input_file_name;

int main(int argc, char* argv[], char* envp[])
{

	ParseProgramOptions(argc, argv);
	//if(argc!=3) {
	//	cout << "Usage: " << argv[0] << "  <inp-file> <namespace_name>" << endl;
	//	exit(0);
	//}
	Init();
	//input_file_name = argv[1];
	//rhs_name_space_name=argv[2];
	
	std::string output_code_directory_prefix = "output/CppCodeGenerator/";
	//PostgreSQLCodeGenerator psqlCodeGenerator;
	PostgreSQLCodeGeneratorFactory psqlFactory; 
	WtUIGeneratorFactory wtFactory;
	CSharpAspnetCodeGeneratorFactory
		cSharpAspNetCodeGeneratorFactory(&psqlFactory, &wtFactory,
						 output_code_directory_prefix);
	CppCodeGeneratorFactory
		cppCodeGeneratorFactory(&psqlFactory, &wtFactory,
					output_code_directory_prefix);
	//codeGeneratorFactory = &cSharpAspNetCodeGeneratorFactory;
	codeGeneratorFactory = &cppCodeGeneratorFactory;
	
	
	FILE * yyin=fopen(global_options::input_file_name.c_str(),"r");
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
	tree_root->RunPreCodeGenerationChecks();
	tree_root->GenerateCode(edit_out);
	GraphType g(global_variables::nGraphNodes);
	ConstructGraph(g, tree_root);
}

void Init()
{
	cout << __PRETTY_FUNCTION__ << ", FILE: " << __FILE__ << ", LINE: " << __LINE__ << endl;
	//CreateTableStatementArray = TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance();
	//ptrCreateTableStatementArray = TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance();
	//TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance();
	TableCollectionSingleton::Instance();
	active_scope=new scope();
	active_scope_list.push_back(active_scope);


	using namespace std;
	string dict_name("english-words.95");
	ifstream dict_file(dict_name.c_str());
	if (dict_file.is_open()) {
		typedef istream_iterator<string> string_input;
		copy(string_input(dict_file), string_input(),
				back_inserter(dict));
		cout << "dictionary contains: " << dict.size() << " words"
			<< endl;
		for (int i=0; i<50; ++i) {
			int pos = dict[i].find("'");
			string without_quote = dict[i].substr(0, pos);
			cout << dict[i] << "|" << without_quote << endl;
		}
	}
	//vector <string> dict;
}

namespace global_options {
std::string database_name;
int database_port;
std::string database_password;
std::string project_namespace;
std::string input_file_name;

//char project_namespace[MAX_NAMESPACE_WORD]={"TopLevel_Namespace"};
}

void ParseProgramOptions(int argc, char * argv[])
{

	namespace po = boost::program_options;
	using namespace global_options;
	try {
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce this help message")
		("top-level-namespace-name", po::value<string>(&project_namespace)->default_value(string("top_level_namespace")), "Top level namespace of the project - for example MyProject")
		("database-name,n", po::value<string>(&database_name)->default_value(string("nxd")), "name of the database to connect to")
		("database-password,w", po::value<string>(&database_password)->default_value(string("nxd")), "database password")
		("database-port,p", po::value<int>(&database_port)->default_value(5432), "port number the database is running on")
		("input-file,f", po::value<string>(&input_file_name), "input file");
	po::positional_options_description p;
	p.add("input-file", -1);
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << "Usage " << argv[0] << " [options]\n";
		cout << desc;
	}

	//if(argc!=3) {
	//	cout << "Usage: " << argv[0] << "  <inp-file> <namespace_name>" << endl;
	//	exit(0);
	//}
	cout << "top-level-namespace-name: " << project_namespace << endl;
	cout << "database_port:" << database_port << endl;
	cout << "database_password:" << database_password << endl;
	cout << "database_name:" << database_name << endl;
	cout << "input_file:" << input_file_name << endl;
	}

	catch (std::exception & e) {
		cout << e.what() << endl;
		return ;
	}

	//exit(0);
	
}
