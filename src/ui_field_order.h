#ifndef UI_FIELD_ORDER_H
#define UI_FIELD_ORDER_H

#include "std_headers.h"
#include "ForwardDecl.h"

	struct UiFieldOrderType 
	{
		std::vector<std::string> uiFieldGroups_;
		std::multimap<std::string, std::vector<var_list*> > uiFieldGroupVars_;
		UiFieldOrderType():
			uiFieldGroups_(), uiFieldGroupVars_()
		{ }
		void clear() 
		{ uiFieldGroups_.clear(); uiFieldGroupVars_.clear(); }
		UiFieldOrderType(const UiFieldOrderType& p_UiFieldOrder){
			uiFieldGroupVars_ = p_UiFieldOrder.uiFieldGroupVars_;
			uiFieldGroups_ = p_UiFieldOrder.uiFieldGroups_;
		}

		private:
		UiFieldOrderType& operator= (const UiFieldOrderType&);
	};
#endif /* UI_FIELD_ORDER_H */
