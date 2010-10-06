#include "std_headers.h"
#include "WtUIGenerator.h"

#include "std_using.h"


WtUIGenerator::WtUIGenerator(TableInfoType * p_tabInfo,
						 std::string & p_output_dir_path)
	//: tableInfo_(p_tabInfo),
	//  outputDirPrefix_(p_output_dir_path)			
	:AbstractUIGenerator(p_tabInfo, p_output_dir_path)
{ }

void WtUIGenerator::GenerateCode()
{
	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;

	cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
}
