#ifndef  WT_UI_GENERATOR_H
#define WT_UI_GENERATOR_H

#include "AbstractUIGenerator.h"

struct WtUIGenerator : public AbstractUIGenerator
{
public:
	WtUIGenerator(TableInfoType * p_tabInfo,
				std::string & p_output_dir_path);
	virtual void GenerateCode();
};

#endif /* WT_UI_GENERATOR_H */
