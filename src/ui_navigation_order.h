#ifndef UI_NAVIGATION_ORDER_H
#define UI_NAVIGATION_ORDER_H

#include "std_headers.h"
#include "ForwardDecl.h"

	struct UiNavigationOrderType 
	{
		std::vector<std::string> uiGroups_;
		std::multimap<std::string, TableInfoType*> uiGroupLinks_;
		UiNavigationOrderType():
			uiGroups_(), uiGroupLinks_()
		{ }
		private:
		UiNavigationOrderType(const UiNavigationOrderType&);
		UiNavigationOrderType& operator= (const UiNavigationOrderType&);
	};
#endif /* UI_NAVIGATION_ORDER_H */
