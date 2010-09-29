#include "stmt.h"
#include "tree.h"
#include "TableCollectionSingleton.hpp"
//#include "CSharpAspNetCodeGenerator.h"
#include "AbstractCodeGenerator.h"
#include "AbstractCodeGeneratorFactory.h"

using std::string;

//template <typename T> TableCollectionSingleton<T> * TableCollectionSingleton::pInstance_=0;
/*
template <typename T>
TableCollectionSingleton<T>& TableCollectionSingleton<T>::Instance()
{
	static TableCollectionSingleton<T> obj;
	return obj;
	//if (pInstance_==0) {
	//	pInstance_ = new TableCollectionSingleton;
	//}
}
*/


table_decl_stmt::table_decl_stmt( datatype dtype, int lline_number, char * & name
		,  struct var_list* & v_list, AbstractCodeGeneratorFactory * p_codeGeneratorFactory
		,  vector<var_list*>& vec_var_list )
	: stmt(dtype, lline_number), codeGenerator_(0)
{
	if ( active_scope->sym_tab.find(name) == active_scope->sym_tab.end() ){
		//cout << "got func_decl" << endl;
		struct TableInfoType* ti=new TableInfoType(name, v_list, vec_var_list );
		//CSharpAspNetCodeGenerator * ptr = new CSharpAspNetCodeGenerator(ti);
		codeGenerator_ = p_codeGeneratorFactory->CreateCodeGenerator(ti);
		//table_info_table.push_back(ti);
		//TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance().Tables.push_back(ti);
		//(TableCollectionSingleton<CSharpAspNetCodeGenerator>::Instance()).Tables.push_back(ptr);
		TableCollectionSingleton::Instance().Tables.push_back(codeGenerator_);
		type=TABLE_TYPE;
		struct symtab_ent* se=new struct symtab_ent;
		if(! se) {
			cerr << "memory allocation error: I will eventually crash :-(" << endl;
		}
		se->name = name;
		string s(name);
		active_scope->sym_tab[s] = se;
		se->type=TABLE_TYPE;
		//tableInfo_=ti;
	} else {
		cerr << "Symbol : " << name << " already present in symbol table" << endl;
		cout << "line_no: " << lline_number;
		++no_errors;
		type=ERROR_TYPE;
	}
}


void table_decl_stmt::GenerateCode(FILE * & fptr)
{
	fprintf(stderr, "ENTER: %s %s: %d\n", __FILE__
			, __PRETTY_FUNCTION__, __LINE__);
	fflush(fptr);
	if(fptr){
		//tableInfo_->print(fptr);
		codeGenerator_->GenerateCode(fptr);
		if(prev) prev->GenerateCode(fptr);
	}
}
