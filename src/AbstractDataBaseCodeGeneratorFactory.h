#ifndef ABSTRACT_DATABASE_CODEGENERATORFACTORY_H
#define ABSTRACT_DATABASE_CODEGENERATORFACTORY_H

#include "std_headers.h"
#include "AbstractDataBaseCodeGenerator.h"
#include "tree.h"


struct AbstractDataBaseCodeGeneratorFactory
{
	AbstractDataBaseCodeGeneratorFactory()
	{ }
	virtual AbstractDataBaseCodeGenerator *
	CreateCodeGenerator(TableInfoType* p_tabInfo,
			    std::string & p_output_dir_path)=0;
private:
	AbstractDataBaseCodeGeneratorFactory(
		const AbstractDataBaseCodeGeneratorFactory&);
	AbstractDataBaseCodeGeneratorFactory& operator=(
		const AbstractDataBaseCodeGeneratorFactory&);
protected:
	~AbstractDataBaseCodeGeneratorFactory()
	{ }
};

#endif /* ABSTRACT_DATABASE_CODEGENERATORFACTORY_H */
