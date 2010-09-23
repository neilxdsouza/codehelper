#include "utils.h"

#include <string>
using std::string;

bool isOfStringType(datatype dt)
{
	if (dt==NVARCHAR_TYPE
		||dt==VARCHAR_TYPE 
		||dt==NTEXT_TYPE 
		||dt==TEXT_TYPE 
		||dt==NCHAR_TYPE) {
		return true;
	} else 
		return false;
}


void print_csharp_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case IMAGE_TYPE:
		fprintf(fptr, "Image");
		break;
		case TINYINT_TYPE:
		fprintf(fptr, "byte");
		break;
			
		case BIGINT_TYPE:{
		fprintf(fptr, "Int64");
		}
		break;		
		case INT32_TYPE:{
		fprintf(fptr, "int");
		}
		break;		
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
		case NTEXT_TYPE:
		case TEXT_TYPE:
			fprintf(fptr, "string");
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "float");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "bool");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "DateTime");
		}
		break;
		default:
			fprintf(fptr, "Error in parsing type file: %s, line:%d\n", __FILE__, __LINE__);
			fprintf(stderr, "case statement datatype not defined\nError in parsing type file: %s, line:%d\n", __FILE__, __LINE__);
	}
}


void print_csharp_db_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case IMAGE_TYPE:
		fprintf(fptr, "Image");
		break;	
		case INT32_TYPE:{
		fprintf(fptr, "Int");
		}
		break;		
		case VARCHAR_TYPE:{
		fprintf(fptr, "VarChar");
		}
		break;		
		case NVARCHAR_TYPE:{
		fprintf(fptr, "NVarChar");
		}
		break;		
		case NCHAR_TYPE:{
		fprintf(fptr, "NChar");
		}
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "Float");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "Double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "Bit");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "DateTime");
		}
		   break;
		case BIGINT_TYPE:
		fprintf(fptr, "BigInt");
		break;
		case NTEXT_TYPE:
		fprintf(fptr, "Ntext");
		break;
		case TEXT_TYPE:
		fprintf(fptr, "Text");
		break;
		case TINYINT_TYPE:
		fprintf(fptr, "TinyInt");
		break;
		default:
			fprintf(fptr, "Unknown Type: Error: File:%s, Line: %d\n", __FILE__, __LINE__);
	}
}


extern vector <TableInfoType*> table_info_table;
struct TableInfoType* my_find_table( string ref_table_name)
{
	for(int i=0; i< table_info_table.size(); ++i){
		if(table_info_table[i]->tableName_==ref_table_name){
			return table_info_table[i];
		}
	}
	return 0;
}


void print_sp_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case INT32_TYPE:{
		fprintf(fptr, "int");
		}
		break;		
		case NCHAR_TYPE:
		fprintf(fptr, "nchar");
		break;		
		case VARCHAR_TYPE:
		fprintf(fptr, "varchar");
		break;		
		case NVARCHAR_TYPE:
		fprintf(fptr, "nvarchar");
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "float");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "bit");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "datetime");
		}
		break;
		case TINYINT_TYPE:
		fprintf(fptr, "tinyint");
		break;
		case TEXT_TYPE:
		fprintf(fptr, "text");
		break;
		case NTEXT_TYPE:
		fprintf(fptr, "ntext");
		break;
		case BIGINT_TYPE:
		fprintf(fptr, "bigint");
		break;
		case IMAGE_TYPE:
		fprintf(fptr, "image");
		break;
		default:
		fprintf(fptr, "Invalid data type: file: %s, line: %d\n", __FILE__, __LINE__  );
	}
}


void print_aspx_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case IMAGE_TYPE:
		fprintf(fptr, "Object");
		break;	
		case TINYINT_TYPE:
		fprintf(fptr, "Byte");
		break;		
		case BIGINT_TYPE:
		fprintf(fptr, "Int64");
		break;		
		
		case INT32_TYPE:{
		fprintf(fptr, "Int32");
		}
		break;		
		case NTEXT_TYPE:
		case TEXT_TYPE:
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
		fprintf(fptr, "String");
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "Single");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "Double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "Boolean");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "DateTime");
		}
		break;
		default:
			fprintf(fptr, "Unknown Type: Error: File:%s, Line: %d\n", __FILE__, __LINE__);
	}
}
