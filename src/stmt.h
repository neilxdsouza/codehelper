
#ifndef _xtcc_stmt_h
#define _xtcc_stmt_h

#include <cstdio>
#include <cstdlib>
#include <vector>

#include "scope.h"
#include "symtab.h"
#include "ForwardDecl.h"

	int search_for_func(string& search_for);
	extern scope* active_scope;
	extern int no_errors;
	//extern vector <TableInfoType*> table_info_table;

//template <typename T>
//class TablesSingleton;
template <typename T>
class TablesSingleton
{
public:
	//static TablesSingleton<T>& Instance();
	static TablesSingleton<T>& Instance()
	{
		static TablesSingleton<T> obj;
		return obj;
		//if (pInstance_==0) {
		//	pInstance_ = new TablesSingleton;
		//}
	}
	std::vector<T*> Tables;
	//template <typename T>
	T* my_find_table(std::string & p_table_name);
private:
	
	TablesSingleton()
		: Tables()
	{ }
	TablesSingleton(const TablesSingleton&);
	//static TablesSingleton * pInstance_;

};

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
	//struct TableInfoType * tableInfo_;
	CSharpAspNetCodeGenerator * codeGenerator_;

	table_decl_stmt( datatype dtype, int lline_number, char * & name,  struct var_list* & v_list);

	void GenerateCode(FILE * & fptr);

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
