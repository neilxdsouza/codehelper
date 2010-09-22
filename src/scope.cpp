
#include "symtab.h"
#include "tree.h"
#include "stmt.h"
#include <string>
#include <cstring>
#include "scope.h"
using namespace std;

extern int line_no;
extern int no_errors;
#if 0
stmt* scope::insert(string name, datatype dt, int line_no){
	// we have to handle a case here where symbol is a function name: - this is not allowed
	struct decl_stmt * st_ptr=new struct decl_stmt(dt, line_no);
	if(st_ptr){
	} else {
		cerr << "Memory allocation failed : line_no" << line_no << endl;
		exit(1);
	}
	if ( sym_tab.find(name) == sym_tab.end() ){
		struct symtab_ent* se=new struct symtab_ent;
		se->name = strdup(name.c_str());
		se->type=dt;
		sym_tab[name] = se;
		st_ptr->type=dt;
		st_ptr->symp=se;
	} else {
		cerr << "ERROR: " << name << " already present in symbol table" << endl;
		st_ptr->type=ERROR_TYPE;
		++no_errors;
	}
	return st_ptr;
}


stmt* scope::insert(string name, datatype dt, int arr_size, int line_no){
	// we have to handle a case here where symbol is a function name: - this is not allowed
	struct decl_stmt * st_ptr=new struct decl_stmt(dt, line_no);
	if(st_ptr){
	} else {
		cerr << "Memory allocation failed : line_no" << line_no << endl;
		exit(1);
	}
	if ( sym_tab.find(name) == sym_tab.end() ){
		struct symtab_ent* se=new struct symtab_ent;
		se->name = strdup(name.c_str());
		se->type=dt;
		se->n_elms=arr_size;
		string s(name);
		sym_tab[s] = se;
		st_ptr->type=dt;
		st_ptr->symp=se;
	} else {
		cerr << " array NAME failed:" << line_no << endl;
		cerr << name << " already present in symbol table" << endl;
		st_ptr->type=ERROR_TYPE;
		++no_errors;
	}
	return st_ptr;
}


stmt* scope::insert(string name, datatype dt, int arr_size, int line_no, char *text){
	// we have to handle a case here where symbol is a function name: - this is not allowed
	int text_len=strlen(text);
	if(arr_size<text_len-1) {
		cerr << "length of TEXT < array size line_no:" << line_no << endl;
		++no_errors;
	}
	struct decl_stmt * st_ptr=new struct decl_stmt(dt, line_no);
	if(st_ptr){
	} else {
		cerr << "Memory allocation failed : line_no" << line_no << endl;
		exit(1);
	}
	if ( sym_tab.find(name) == sym_tab.end() ){
		cout << "char decl:start\n";
		struct symtab_ent* se=new struct symtab_ent;
		se->name = strdup(name.c_str());
		se->type=dt;
		se->n_elms=arr_size;
		se->text=strdup(text);
		string s(name);
		sym_tab[s] = se;
		st_ptr->type=dt;
		st_ptr->symp=se;
	} else {
		cerr << " array NAME failed:" << line_no << endl;
		cerr << name << " already present in symbol table" << endl;
		st_ptr->type=ERROR_TYPE;
		++no_errors;
	}
	return st_ptr;
}

#endif /* 0 */
