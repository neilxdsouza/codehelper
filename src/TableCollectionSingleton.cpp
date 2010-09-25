#include "TableCollectionSingleton.hpp"

AbstractCodeGenerator* TableCollectionSingleton::my_find_table(std::string & p_table_name)
{
	//fprintf(stderr, "%s %s : %d, p_table_name: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__,
	//		p_table_name.c_str());
	for (int i=0; i<Tables.size(); ++i) {
		if(Tables[i]->tableInfo_->tableName_==p_table_name){
			return Tables[i];
		}
	}
	return 0;
}
