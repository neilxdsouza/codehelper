#include "std_headers.h"
#include "stmt.h"
#include "tree.h"
#include "TableCollectionSingleton.hpp"
//#include "CSharpAspNetCodeGenerator.h"
#include "AbstractCodeGenerator.h"
#include "AbstractCodeGeneratorFactory.h"
#include "global_variables.h"
#include "error.h"

using std::string;
vector <TableInfoType *> vec_table_info;

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

table_decl_stmt::table_decl_stmt( datatype dtype, int lline_number, char * & name,
		struct var_list* & v_list, AbstractCodeGeneratorFactory * p_codeGeneratorFactory,
		vector<var_list*>& vec_var_list, struct tab_level_options_list_type & p_tab_options,
		multimap<string, vector<var_list*> > & p_field_groups,
		UiFieldOrderType & p_ui_field_order
		)
	: stmt(dtype, lline_number), codeGenerator_(0), tab_options(p_tab_options), 
		field_groups(p_field_groups), ui_field_order(p_ui_field_order)
{
	// cout << __PRETTY_FUNCTION__ << "," << __FILE__ << "," << __LINE__ << endl;
	if ( active_scope->sym_tab.find(name) == active_scope->sym_tab.end() ){
		struct TableInfoType* ti=new TableInfoType(name, v_list, vec_var_list, tab_options, p_field_groups,
				p_ui_field_order);
		codeGenerator_ = p_codeGeneratorFactory->CreateCodeGenerator(ti);
		TableCollectionSingleton::Instance().Tables.push_back(codeGenerator_);
		if (ti->tab_options.has_ui_group_name) {
			multimap<string, TableInfoType*> & m = global_variables::ui_navigation_order.uiGroupLinks_;
			vector<string> & v = global_variables::ui_navigation_order.uiGroups_;
			if (m.find(ti->tab_options.ui_group_name) == m.end()) {
				v.push_back(ti->tab_options.ui_group_name);
				//m[ti->tab_options.ui_group_name] = ti;
				m.insert(make_pair(ti->tab_options.ui_group_name, ti));
			} else {
				m.insert(make_pair(ti->tab_options.ui_group_name, ti));
			}
		}
		type=TABLE_TYPE;
		struct symtab_ent* se=new struct symtab_ent;
		if(! se) {
			cerr << "memory allocation error: I will eventually crash :-(" << endl;
		}
		se->name = name;
		string s(name);
		active_scope->sym_tab[s] = se;
		se->type=TABLE_TYPE;
		vec_table_info.push_back(ti);
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
	//codeGenerator_->FinalCleanUp();
}

void table_decl_stmt::RunPreCodeGenerationChecks()
{
	//struct TableInfoType * ti_ptr = codeGenerator_->tableInfo_;
	//struct var_list *v_ptr = ti_ptr->param_list;
	//// Make this a virtual function and implement in CppCodeGenerator
	//while (v_ptr) {
	//	if (v_ptr->options.many) {
	//			struct CppCodeGenerator* tbl_ptr = (dynamic_cast<CppCodeGenerator*>
	//					(TableCollectionSingleton::Instance()
	//						.my_find_table(v_ptr->options.ref_table_name)));
	//	}
	//	v_ptr = v_ptr->prev;
	//}
	codeGenerator_->RunPreCodeGenerationChecks();
	if (prev) {
		prev->RunPreCodeGenerationChecks();
	}
}
