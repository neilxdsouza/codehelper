#ifndef _SYMTAB_H
#define _SYMTAB_H
#include <iostream>

	enum datatype {
		/*
		VOID_TYPE, 
		U_INT8_TYPE, INT8_TYPE, U_INT16_TYPE, INT16_TYPE, U_INT32_TYPE, INT32_TYPE, FLOAT_TYPE, DOUBLE_TYPE,
		U_INT8_ARR_TYPE, INT8_ARR_TYPE, U_INT16_ARR_TYPE, INT16_ARR_TYPE, U_INT32_ARR_TYPE, INT32_ARR_TYPE, FLOAT_ARR_TYPE, DOUBLE_ARR_TYPE,
		U_INT8_REF_TYPE, INT8_REF_TYPE, U_INT16_REF_TYPE, INT16_REF_TYPE, U_INT32_REF_TYPE, INT32_REF_TYPE, FLOAT_REF_TYPE, DOUBLE_REF_TYPE,
		ERROR_TYPE,
		TEXPR_STMT, CMPD_STMT, IFE_STMT, DECL_STMT, FUNC_TYPE, FUNC_DEFN, FOR_STMT,
		LISTA_BASIC_TYPE_STMT, LISTA_BASIC_ARRTYPE_STMT_1INDEX, LISTA_BASIC_ARRTYPE_STMT_2INDEX,
		BREAK_STMT, CONTINUE_STMT,
		UNINIT */
		TABLE_TYPE, VARCHAR_TYPE,  INT32_TYPE, BIGINT_TYPE, FLOAT_TYPE, DOUBLE_TYPE, ERROR_TYPE, BIT_TYPE, DATETIME_TYPE,
		NVARCHAR_TYPE, NTEXT_TYPE, TEXT_TYPE, TINYINT_TYPE, IMAGE_TYPE, NCHAR_TYPE, NOT_INITIALIZED_TYPE, COMPOSITE_TYPE
	};

	struct symtab_ent {
		char *name;
		char *text;
		int n_elms;
		//union {
			double dval;
			int ival;
		//};
		datatype type;
		symtab_ent() 
			: name(0), text(0), n_elms(0), dval(0.0), ival(0), 
			  type(NOT_INITIALIZED_TYPE)
		{ }
		inline datatype get_type()
		{
			return type;
		}
	};


#endif /* _SYMTAB_H */
