#include "stmt.h"
#include "tree.h"
#include "CSharpAspNetCodeGenerator.h"

using std::string;

//template <typename T> TablesSingleton<T> * TablesSingleton::pInstance_=0;
/*
template <typename T>
TablesSingleton<T>& TablesSingleton<T>::Instance()
{
	static TablesSingleton<T> obj;
	return obj;
	//if (pInstance_==0) {
	//	pInstance_ = new TablesSingleton;
	//}
}
*/

template <typename T>
T* TablesSingleton<T>::my_find_table(string & p_table_name)
{
	for (int i=0; i<Tables.size(); ++i) {
		if(Tables[i]->tableInfo_->tableName_==p_table_name){
			return Tables[i];
		}
	}
}


table_decl_stmt::table_decl_stmt( datatype dtype, int lline_number, char * & name
		,  struct var_list* & v_list)
	//: stmt(dtype, lline_number), tableInfo_(0)
	: stmt(dtype, lline_number), codeGenerator_(0)
{
	//cout << "load_func_into_symbol_table : " << "name: " << name << endl;
	if ( active_scope->sym_tab.find(name) == active_scope->sym_tab.end() ){
		//cout << "got func_decl" << endl;
		struct TableInfoType* ti=new TableInfoType(name, v_list );
		CSharpAspNetCodeGenerator * ptr = new CSharpAspNetCodeGenerator(ti);
		//table_info_table.push_back(ti);
		//TablesSingleton<CSharpAspNetCodeGenerator>::Instance().Tables.push_back(ti);
		TablesSingleton<CSharpAspNetCodeGenerator>::Instance().Tables.push_back(ptr);
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
		codeGenerator_ = ptr;
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
		codeGenerator_->print(fptr);
		if(prev) prev->GenerateCode(fptr);
	}
}
