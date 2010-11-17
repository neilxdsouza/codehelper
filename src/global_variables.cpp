#include "global_variables.h"
#include "ui_navigation_order.h"


namespace global_variables {

	int nGraphNodes;
	std::vector<int> topologicalOrder;
	std::string output_code_directory_prefix = "output/CppCodeGenerator/";
	UiNavigationOrderType ui_navigation_order;
}
