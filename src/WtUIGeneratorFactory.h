#ifndef WT_UI_GENERATOR_FACTORY_H
#define WT_UI_GENERATOR_FACTORY_H

#include "AbstractUIGeneratorFactory.h"
#include "WtUIGenerator.h"

struct WtUIGeneratorFactory: public AbstractUIGeneratorFactory
{
	WtUIGeneratorFactory()
	{ }
	virtual AbstractUIGenerator *
		CreateCodeGenerator(TableInfoType * p_tabInfo,
				    std::string & p_output_dir_path)
	{
		return new WtUIGenerator(p_tabInfo,
						   p_output_dir_path);
	}
};

#endif /* WT_UI_GENERATOR_FACTORY_H */
