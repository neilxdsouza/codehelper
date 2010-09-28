#ifndef ERROR_H
#define ERROR_H
#include "std_headers.h"

void error(std::string filename, int line_no, std::string function_name,
	   std::string err_msg);

#endif /* ERROR_H */
