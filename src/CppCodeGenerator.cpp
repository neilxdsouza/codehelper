#include <string>
#include "stmt.h"
#include "TableCollectionSingleton.hpp"
#include "CppCodeGenerator.h"
#include "utils.h"
#include <boost/format.hpp>

using boost::format;
using boost::io::group;

void CppCodeGenerator::GenerateCode(FILE * fptr)
{
	//cout << "CppCodeGenerator::GenerateCode()" << endl;
	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;

	std::string output_code_directory_prefix = "output/CppCodeGenerator/";
	
	dbCodeGenerator_->GenerateCode();



	cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
}
