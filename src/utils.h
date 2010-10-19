#ifndef UTILS_H
#define UTILS_H

#include "std_headers.h"
#include <cstdio>
#include "symtab.h"
#include "tree.h"
bool isOfStringType(datatype dt);
void print_csharp_types(FILE * fptr, datatype dt);
void print_csharp_types(stringstream & file, datatype dt);
void print_csharp_db_types(FILE * fptr, datatype dt);
//struct TableInfoType* my_find_table( std::string ref_table_name);
void print_sp_types(FILE * fptr, datatype dt);
void print_aspx_types(FILE * fptr, datatype dt);
std::string print_sp_types(datatype dt);
std::string print_cpp_types(datatype dt);
TableInfoType * find_TableInfo(string name);
bool ReferencedTableContainsUs(TableInfoType *me, std::string ref_table_name);

#endif /* UTILS_H */
