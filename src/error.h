#ifndef ERROR_H
#define ERROR_H
#include "std_headers.h"

void error(std::string filename, int line_no, std::string function_name,
	   std::string err_msg);
void fixme(std::string filename, int line_no, std::string function_name,
	   std::string msg);

#endif /* ERROR_H */
