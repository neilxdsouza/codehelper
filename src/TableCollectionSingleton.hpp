#ifndef TABLE_COLLECTION_SINGLETON
#define TABLE_COLLECTION_SINGLETON


//template <typename T>
//class TableCollectionSingleton;
#include "std_headers.h"
#include <cstdio>
#include "AbstractCodeGenerator.h"

#if 0
template <typename T>
class TableCollectionSingleton
{
public:
	//static TableCollectionSingleton<T>& Instance();
	static TableCollectionSingleton<T>& Instance()
	{
		static TableCollectionSingleton<T> obj;
		//fprintf(stderr, "%s %s : %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		//fprintf(stderr, "obj.Tables.size(): %d\n", obj.Tables.size());
		return obj;
		//if (pInstance_==0) {
		//	pInstance_ = new TableCollectionSingleton;
		//}
	}
	std::vector<T*> Tables;
	//template <typename T>
	T* my_find_table(std::string & p_table_name);
private:
	
	TableCollectionSingleton()
		: Tables()
	{ }
	TableCollectionSingleton(const TableCollectionSingleton&);
	//static TableCollectionSingleton * pInstance_;
};


template <typename T>
T* TableCollectionSingleton<T>::my_find_table(std::string & p_table_name)
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
#endif /* 0 */

class TableCollectionSingleton
{
public:
	static TableCollectionSingleton& Instance()
	{
		static TableCollectionSingleton obj;
		//fprintf(stderr, "%s %s : %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		//fprintf(stderr, "obj.Tables.size(): %d\n", obj.Tables.size());
		return obj;
		//if (pInstance_==0) {
		//	pInstance_ = new TableCollectionSingleton;
		//}
	}
	std::vector<AbstractCodeGenerator*> Tables;
	AbstractCodeGenerator* my_find_table(std::string & p_table_name);
private:
	
	TableCollectionSingleton()
		: Tables()
	{ }
	TableCollectionSingleton(const TableCollectionSingleton&);
	//static TableCollectionSingleton * pInstance_;
};


#endif /* TABLE_COLLECTION_SINGLETON */
