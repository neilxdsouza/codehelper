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
void print_sp_types(ofstream& file, datatype dt);

#endif /* UTILS_H */
