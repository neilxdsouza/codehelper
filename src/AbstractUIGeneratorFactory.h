#ifndef ABSTRACT_UI_GENERATOR_FACTORY_H
#define ABSTRACT_UI_GENERATOR_FACTORY_H


#include <string>
#include "tree.h"


struct AbstractUIGeneratorFactory
{

	AbstractUIGeneratorFactory()
	{ }
	virtual AbstractUIGenerator*
		CreateCodeGenerator(TableInfoType * p_TableInfoType_ptr,
				    std::string & p_output_dir_path)=0;
	
protected:
	~AbstractUIGeneratorFactory()
	{ }
private:
	AbstractUIGeneratorFactory(const AbstractUIGeneratorFactory&  );
	AbstractUIGeneratorFactory& operator=(const AbstractUIGeneratorFactory&);
};

#endif  /* ABSTRACT_UI_GENERATOR_FACTORY_H */
