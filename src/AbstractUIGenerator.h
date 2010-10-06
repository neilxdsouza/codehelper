#ifndef ABSTRACT_UI_GENERATOR_H
#define ABSTRACT_UI_GENERATOR_H

#include <string>
#include "tree.h"

struct AbstractUIGenerator
{
public:
	TableInfoType * tableInfo_;
	std::string outputDirPrefix_;
	AbstractUIGenerator(TableInfoType * p_tabInfo,
				std::string & p_output_dir_path)
	: tableInfo_(p_tabInfo),
	  outputDirPrefix_(p_output_dir_path)
	{ }
	virtual void GenerateCode()=0;

	//virtual void SetOutputDirectory();
	//virtual void SetTableInfo();
protected:
	~AbstractUIGenerator()
	{ }
private:
	AbstractUIGenerator(const AbstractUIGenerator&);
	AbstractUIGenerator& operator=(const AbstractUIGenerator&);


};

#endif /* ABSTRACT_UI_GENERATOR_H */
