
#ifndef _xtcc_stmt_h
#define _xtcc_stmt_h
#include "scope.h"
#include "symtab.h"

#include <cstdio>
#include <cstdlib>

	int search_for_func(string& search_for);
	extern scope* active_scope;
	extern int no_errors;
	extern vector <TableInfoType*> table_info_table;

// Note : I may have to add file name we are compiling very soon
struct stmt
{
	struct stmt * next;
	struct stmt * prev;
	datatype type;
	int line_number;
	virtual void GenerateCode(FILE * & fptr)=0;
	stmt(datatype dtype=ERROR_TYPE, int lline_number=0)
		:next(0), prev(0), type(dtype), line_number(lline_number)
	{}
	virtual ~stmt(){}
	private:
		stmt(const stmt&);
		stmt& operator=(const stmt&);
};


// Refinement3


struct err_stmt: public stmt{
	err_stmt( int lline_number): stmt(ERROR_TYPE, lline_number){}
	void GenerateCode(FILE * & fptr){
		fflush(fptr);

		if(fptr){
			fprintf(fptr, "error");
			if(prev) prev->GenerateCode(fptr);
		}
	}
	private:
	err_stmt& operator=(const err_stmt&);	
	err_stmt(const err_stmt&);	
};




struct table_decl_stmt: public stmt
{
	struct TableInfoType * tableInfo_;

	table_decl_stmt( datatype dtype, int lline_number, char * & name,  struct var_list* & v_list):
		stmt(dtype, lline_number), tableInfo_(0)
	{
		//cout << "load_func_into_symbol_table : " << "name: " << name << endl;
		if ( active_scope->sym_tab.find(name) == active_scope->sym_tab.end() ){
			//cout << "got func_decl" << endl;
			struct TableInfoType* ti=new TableInfoType(name, v_list );
			table_info_table.push_back(ti);
			type=TABLE_TYPE;
			struct symtab_ent* se=new struct symtab_ent;
			if(! se) {
				cerr << "memory allocation error: I will eventually crash :-(" << endl;
			}
			se->name = name;
			string s(name);
			active_scope->sym_tab[s] = se;
			se->type=TABLE_TYPE;
			tableInfo_=ti;
		} else {
			cerr << "Symbol : " << name << " already present in symbol table" << endl;
			cout << "line_no: " << lline_number;
			++no_errors;
			type=ERROR_TYPE;
		}
	}
	void GenerateCode(FILE * & fptr){
		fflush(fptr);

		if(fptr){
			tableInfo_->print(fptr);
			if(prev) prev->GenerateCode(fptr);
		}
	}
	private:
	table_decl_stmt& operator=(const table_decl_stmt&);	
	table_decl_stmt(const table_decl_stmt&);	
};

#if 0
struct decl_stmt: public stmt{
	struct symtab_ent* symp;
	decl_stmt( datatype dtype, int lline_number):stmt(dtype, lline_number),symp(0)
	{}
	void GenerateCode(FILE * & fptr){
		fflush(fptr);

		if(fptr){
			if(type >= U_INT8_TYPE && type <=DOUBLE_TYPE){
				fprintf(fptr,"%s %s;\n", noun_list[type].sym, symp->name);
			} else if (type >=U_INT8_ARR_TYPE && type <=DOUBLE_ARR_TYPE){
				datatype tdt=datatype(U_INT8_TYPE + type-U_INT8_ARR_TYPE);
				fprintf(fptr,"%s %s [ %d ];\n", noun_list[tdt].sym, symp->name, symp->n_elms);
			} else if (type >=U_INT8_REF_TYPE&& type <=DOUBLE_REF_TYPE){
				datatype tdt=datatype(U_INT8_TYPE + type-U_INT8_REF_TYPE);
				fprintf(fptr,"%s & %s;\n", noun_list[tdt].sym, symp->name);
			}
			if(prev) prev->GenerateCode(fptr);
		}

	}
	private:
	decl_stmt& operator=(const decl_stmt&);	
	decl_stmt(const decl_stmt&);	
};
#endif /* 0 */

#endif /* _xtcc_stmt_h */
