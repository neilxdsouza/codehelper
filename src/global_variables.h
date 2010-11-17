#ifndef __GLOBAL_VARIABLES_H
#define __GLOBAL_VARIABLES_H

#include "std_headers.h"
#include "ForwardDecl.h"
#include "ui_navigation_order.h"

namespace global_variables {
	extern int nGraphNodes;
	extern std::vector<int> topologicalOrder;
	extern std::string output_code_directory_prefix;

	extern UiNavigationOrderType ui_navigation_order;
}

#endif /* __GLOBAL_VARIABLES_H */
