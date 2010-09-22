
#ifndef __SCOPE_H
#define __SCOPE_H

#include <map>
#include <string>

//#include "defs.h"
#include <iostream>
#include <vector>

using namespace std;

	struct scope;
	//extern vector<scope*> active_scope_list;

#include "symtab.h"

	struct scope{
		map<string,symtab_ent*> sym_tab;
		scope(): sym_tab()	
		{
			//cout << " constructing scope(): this: " << this << endl;
		}
#if 0
		struct stmt* insert(string name, datatype dt, int line_no);
		struct stmt* insert(string name, datatype dt, int arr_size, int line_no);
		struct stmt* insert(string name, datatype dt, int arr_size, int line_no, char *text);
#endif /* 0 */
	};

#endif /* __SCOPE_H */
