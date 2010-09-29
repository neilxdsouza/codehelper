#include "std_headers.h"
#include "error.h"
#include "std_using.h"

void error(string filename, int line_no, string function_name, string err_msg)
{
	std::cerr << "Error "
	     << ", filename: " << filename
	     << ", line no: " << line_no
	     << ", function: " << function_name
	     << ", err_msg: " << err_msg
	     << std::endl;
	exit(1);
}

void fixme(std::string filename, int line_no, std::string function_name,
	   std::string msg)
{
	std::cerr << "FIXME "
	     << ", filename: " << filename
	     << ", line no: " << line_no
	     << ", function: " << function_name
	     << ", msg: " << msg
	     << std::endl;
}
