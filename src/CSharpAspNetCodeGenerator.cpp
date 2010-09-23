#include "CSharpAspNetCodeGenerator.h"
#include "utils.h"

extern char project_namespace[];
extern int line_no;
extern vector <CSharpAspNetCodeGenerator*> table_info_table;
extern string rhs_name_space_name;
static struct CSharpAspNetCodeGenerator* my_find_table( string ref_table_name);

void CSharpAspNetCodeGenerator::print_sql_provider(FILE * fptr)
{
	print_sql_provider_header(fptr);

	print_sql_provider_get_index_size(fptr);
	print_sql_provider_get_count(fptr);
	fprintf(fptr, "\n\n");


	print_sql_provider_get_by_code(fptr);

	fprintf(fptr, "\n\n");
	print_sql_provider_delete(fptr);
	fprintf(fptr, "\n\n");
	print_sql_provider_insert(fptr);
	fprintf(fptr, "\n\n");
	print_sql_provider_update(fptr);
	// print any unique checks if required
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.unique) {
			fprintf(fptr, "\tpublic override int UniqueCheck_%s(",
				v_ptr->var_name.c_str());
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s){\n", v_ptr->var_name.c_str());
			fprintf(fptr, "\t\tusing (SqlConnection cn = new SqlConnection(this.ConnectionString)){\n");
			fprintf(fptr, "\t\t\tSqlCommand cmd = new SqlCommand(\"srp_%s_UniqueCheck_%s\", cn);\n",
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t\tcmd.CommandType = CommandType.StoredProcedure;\n");

			fprintf(fptr, "\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", v_ptr->var_name.c_str());
			print_csharp_db_types(fptr, v_ptr->var_type);
			fprintf(fptr, ").Value = ");
			fprintf(fptr, "l_%s;\n", v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t\tcn.Open();\n");
			fprintf(fptr, "\t\t\treturn (int)ExecuteScalar(cmd);\n");
			fprintf(fptr, "\t\t}\n");
			fprintf(fptr, "\t}\n\n");

		}
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, "\n\n");
	fprintf(fptr, "\t}\n");
	fprintf(fptr, "}\n");
}


void CSharpAspNetCodeGenerator::print_abstract_provider(FILE * fptr){
	fprintf(fptr, 
		"using System;\n"
		"using System.Data;\n"
		"using System.Configuration;\n"
		"using System.Web;\n"
		"using System.Web.Security;\n"
		"using System.Web.UI;\n"
		"using System.Web.UI.WebControls;\n"
		"using System.Web.UI.WebControls.WebParts;\n"
		"using System.Web.UI.HtmlControls;\n"
		"using System.Collections.Generic;\n\n"
		"using %s.BLL.%s;\n\n", project_namespace, rhs_name_space_name.c_str()
		);

	fprintf(fptr, "namespace %s.DAL.%s {\n", project_namespace, rhs_name_space_name.c_str());
	fprintf(fptr, "public abstract class %sProvider: DataAccess{\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\tstatic private %sProvider _Instance=null;\n", tableInfo_->tableName_.c_str() );
	fprintf(fptr, "\tstatic public %sProvider Instance{\n", tableInfo_->tableName_.c_str() );
	fprintf(fptr, "\t\t/* get {\n\t\t\tif(_Instance==null) {\n\t\t\t\t _Instance=(%sProvider)Activator.CreateInstance(Type.GetType(Globals.Settings.%s.ProviderType));\n\t\t\t}\n\t\t\treturn _Instance;\n\t\t}*/\n", 
			tableInfo_->tableName_.c_str(),
			tableInfo_->tableName_.c_str()
			);
	fprintf(fptr, "\t\t get {\n\t\t\tif(_Instance==null) {\n\t\t\t\t _Instance=(%sProvider)Activator.CreateInstance(Type.GetType(\"%s.DAL.SQLClient.Sql%sProvider\"));\n\t\t\t}\n\t\t\treturn _Instance;\n\t\t}\n", 
			tableInfo_->tableName_.c_str(), project_namespace,
			tableInfo_->tableName_.c_str()
			);

	fprintf(fptr, "\t}\n");
	fprintf(fptr, "\t/*\n");
	fprintf(fptr, "\tpublic %sProvider()", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t{\n");
	fprintf(fptr, "\t\tthis.ConnectionString = Globals.Settings.%s.ConnectionString;\n", tableInfo_->tableName_.c_str());;
	fprintf(fptr, "\t\tthis.EnableCaching = Globals.Settings.%s.EnableCaching;\n",tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\tthis.CacheDuration = Globals.Settings.%s.CacheDuration;\n",tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t}\n");
	fprintf(fptr, "\t\n");

	fprintf(fptr, "\tpublic %sProvider()", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t{\n");
	fprintf(fptr, "\t\tthis.ConnectionString = Globals.Settings.Commons.ConnectionString;\n");;
	fprintf(fptr, "\t\tthis.EnableCaching = Globals.Settings.Commons.EnableCaching;\n");
	fprintf(fptr, "\t\tthis.CacheDuration = Globals.Settings.Commons.CacheDuration;\n");
	fprintf(fptr, "\t}*/\n");

	fprintf(fptr, "\tpublic abstract List<Biz%s> Get%ss(int pageIndex, int pageSize\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	bool comma_is_pending = false;
	if(tableInfo_->has_search_key>0){
		comma_is_pending=true;
	}
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.search_key) {
			if(comma_is_pending){
				fprintf(fptr, ", ");
				comma_is_pending=false;
			}
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			comma_is_pending=true;
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, ");\n");



	//fprintf(fptr, "\tpublic abstract int Get%ssCount();\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\tpublic abstract int Get%ssCount(", tableInfo_->tableName_.c_str());
	v_ptr=tableInfo_->param_list;
	comma_is_pending = false;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.search_key) {
			if(comma_is_pending){
				fprintf(fptr, ", ");
				comma_is_pending=false;
			}
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			comma_is_pending=true;
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, ");\n");

	fprintf(fptr, "\tpublic abstract Biz%s Get%sByCode( ", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	print_csharp_types(fptr, tableInfo_->param_list->var_type);
	fprintf(fptr, " " );
	print_lower_fname(fptr); fprintf(fptr, ");\n");
	//fprintf(fptr, "\tpublic abstract bool Delete%s", tableInfo_->tableName_.c_str());
	//fprintf(fptr, "(int "); tableInfo_->param_list->print_1st_var(fptr); fprintf(fptr, ");\n");
	fprintf(fptr, "\tpublic abstract int Insert%s(Biz%s ", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	print_lower_fname(fptr);
	fprintf(fptr, "	);\n"); 
	fprintf(fptr, "\tpublic abstract int Update%s(Biz%s ", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	print_lower_fname(fptr);
	fprintf(fptr, "	);\n");
	v_ptr=tableInfo_->param_list;
	while (v_ptr){
		if(v_ptr->options.unique){
			fprintf(fptr, "\tpublic abstract int UniqueCheck_%s(",
				v_ptr->var_name.c_str());
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s);\n", v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
	}

	print_get_single_record(fptr);

	print_get_collection(fptr);

	fprintf(fptr, "\t} /* Close class */\n\n\n");
	fprintf(fptr, "} /* Close namespace */\n\n\n");
}

void CSharpAspNetCodeGenerator:: print_get_single_record(FILE * fptr){
	fprintf(fptr, "\tprotected virtual Biz%s Get%sFromReader(IDataReader reader)", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str()); 
	fprintf(fptr, "\t{\n");
	if(tableInfo_->param_list){
		struct var_list* v_ptr=tableInfo_->param_list;
		// This is for the Object within this current object object
		while (v_ptr) {
			if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false) {
				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				fprintf(fptr, "\t\tBiz%s l_biz_%s=new Biz%s(\n", 
					v_ptr->options.ref_table_name.c_str(),
					improved_name.c_str(),
					v_ptr->options.ref_table_name.c_str()
					//v_ptr->options.ref_table_name.c_str()
					);
				
				print_reader_param_with_cast(fptr, v_ptr);
				fprintf(fptr, ",\n");
				struct CSharpAspNetCodeGenerator* t_ptr = my_find_table(v_ptr->options.ref_table_name);
				t_ptr->print_reader(fptr, false, true,  v_ptr->var_name.c_str());
				fprintf(fptr, "\t\t\t);\n");
				//fprintf(fptr, "\t\tl_%s.%s = l_%s;\n", tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str(), 
				//		v_ptr->var_name.c_str() );
			} else if (v_ptr->options.ref_table_name!="" && v_ptr->options.many==true) {
				fprintf(fptr, "\t\tList<Biz%s> l_%s= new Biz%s();\n", v_ptr->options.ref_table_name.c_str(),
					v_ptr->var_name.c_str(),
					v_ptr->options.ref_table_name.c_str()
					//,
					//v_ptr->options.ref_table_name.c_str()
					);
				fprintf(fptr, "\t\t");
				print_csharp_types(fptr, v_ptr->var_type);
				fprintf(fptr, " pk_l_%s = ", v_ptr->var_name.c_str());
				print_reader_param_with_cast(fptr, v_ptr);
				fprintf(fptr, ";\n");
				fprintf(fptr, "\t\tl_%s = SiteProvider.%s.Get%sByCode(pk_l_%s);\n",
					v_ptr->var_name.c_str(), v_ptr->options.ref_table_name.c_str(),	
					v_ptr->options.ref_table_name.c_str(),
					v_ptr->var_name.c_str()
					);

			}
			v_ptr=v_ptr->prev;
		}
		fprintf(fptr, "\t\tBiz%s l_%s= new Biz%s(\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
		//tableInfo_->param_list->print(fptr);
		print_reader(fptr, true, false,  "");

		fprintf(fptr, "\t\t\t);\n");

		fprintf(fptr, "\t\treturn l_%s;\n", tableInfo_->tableName_.c_str());
	}
	fprintf(fptr, "\n\t}\n");
}

void CSharpAspNetCodeGenerator:: print_get_collection(FILE * fptr){


	fprintf(fptr, "\tprotected virtual List<Biz%s> Get%sCollectionFromReader(IDataReader reader)", 
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str()); 
	fprintf(fptr, "\t{\n");
	fprintf(fptr, "\t\tList <Biz%s> ", tableInfo_->tableName_.c_str()); print_lower_fname(fptr);
	fprintf(fptr, "=new List <Biz%s> ();\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\twhile(reader.Read())\n");
	fprintf(fptr, "\t\t\t"); print_lower_fname(fptr); 
	fprintf(fptr, ".Add(Get%sFromReader(reader));\n", tableInfo_->tableName_.c_str());	
	//fprintf(fptr, "\t\treturn sector;");
	fprintf(fptr, "\t\treturn "); print_lower_fname(fptr); fprintf(fptr, ";\n");

	fprintf(fptr, "\n\t}\n");
}


void CSharpAspNetCodeGenerator:: print(FILE * fptr)
{
#define FNAME_SZ 255
	char myfname[FNAME_SZ];


	/* ======= BLL ===========*/
	sprintf(myfname, "output/%s_BLL.cs", tableInfo_->tableName_.c_str());
	FILE * bll=fopen(myfname, "wb");
	if(!bll){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_bll(bll);
	fclose(bll);
	


	/*========= ABSTRACT PROVIDER =========*/
	sprintf(myfname, "output/%s_abs.cs", tableInfo_->tableName_.c_str());
	FILE * abs=fopen(myfname, "wb");
	if(!abs){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}

	print_abstract_provider(abs);
	fclose(abs);

	sprintf(myfname, "output/%s_sql.cs", tableInfo_->tableName_.c_str());
	FILE * sql=fopen(myfname, "wb");
	if(!sql){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_sql_provider(sql);
	fclose(sql);
	
	

	sprintf(myfname, "output/%s_sp.sql", tableInfo_->tableName_.c_str());
	FILE * sp=fopen(myfname, "wb");
	if(!sp){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_sp(sp);
	fclose(sp);

	sprintf(myfname, "output/Manage%s.aspx", tableInfo_->tableName_.c_str());
	FILE * aspx=fopen(myfname, "wb");
	if(!aspx){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	bool called_recursively=false;
	print_aspx(aspx, called_recursively);
	fclose(aspx);

	sprintf(myfname, "output/Manage%s.aspx.cs", tableInfo_->tableName_.c_str());
	FILE * aspx_cs=fopen(myfname, "wb");
	if(!aspx_cs){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_aspx_cs(aspx_cs);
	fclose(aspx_cs);


	sprintf(myfname, "output/%sList.ascx", tableInfo_->tableName_.c_str());
	FILE * ascx=fopen(myfname, "wb");
	if(!ascx){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_ascx(ascx);
	fclose(ascx);

	sprintf(myfname, "output/%sList.ascx.cs", tableInfo_->tableName_.c_str());
	FILE * ascx_cs=fopen(myfname, "wb");
	if(!ascx){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_ascx_cs(ascx_cs);
	fclose(ascx_cs);

	// ----------
	sprintf(myfname, "output/%sPopup.aspx", tableInfo_->tableName_.c_str());
	FILE * popup_aspx =fopen(myfname, "wb");
	if(!popup_aspx){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_popup_aspx(popup_aspx);

	// ----------
	sprintf(myfname, "output/%sPopup.aspx.cs", tableInfo_->tableName_.c_str());
	FILE * popup_aspx_cs =fopen(myfname, "wb");
	if(!popup_aspx_cs){
		fprintf(stderr, "Error opening file: %s for output ... exiting\n", myfname);
		exit(1);
	}
	print_popup_aspx_cs(popup_aspx_cs);
	
	

}


void CSharpAspNetCodeGenerator::print_insert_dp_params(FILE *fptr){
	struct var_list* v_ptr=tableInfo_->param_list;
	// skip the primary key in the insert - it is added at the end as an output parameter
	// Also assume that primary key is always an integer
	v_ptr=v_ptr->prev;
	while(v_ptr){
		fprintf(fptr, "\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", v_ptr->var_name.c_str());
		print_csharp_db_types(fptr, v_ptr->var_type);
		fprintf(fptr, ").Value = ");
		print_lower_fname(fptr);
		//tableInfo_->param_list->print_1st_var(fptr); 
		fprintf(fptr, ".%s;\n", v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;
	}

	if(tableInfo_->param_list){
		fprintf(fptr, "\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", tableInfo_->param_list->var_name.c_str());
		print_csharp_db_types(fptr, tableInfo_->param_list->var_type);
		fprintf(fptr, ").Direction = ParameterDirection.Output;\n"); 

	}
}

void CSharpAspNetCodeGenerator::print_dp_params(FILE *fptr){
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", v_ptr->var_name.c_str());

		print_csharp_db_types(fptr, v_ptr->var_type);
		fprintf(fptr, ").Value = ");
		//tableInfo_->param_list->print_1st_var(fptr); 
		print_lower_fname(fptr);
		fprintf(fptr, ".%s;\n", v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;
	}
}

void CSharpAspNetCodeGenerator::print_sql_provider_get_index_size(FILE * fptr){

	fprintf(fptr, "\t\tpublic override List<Biz%s> Get%ss(int pageIndex, int pageSize\n",
				tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	if(tableInfo_->has_search_key){
		struct var_list* v_ptr=tableInfo_->param_list;
		bool comma_is_pending = false;
		if(tableInfo_->has_search_key>0){
			comma_is_pending=true;
		}
		while(v_ptr){
			fprintf(fptr, "\n\t\t");
			if(v_ptr->options.search_key) {
				if(comma_is_pending){
					fprintf(fptr, ", ");
					comma_is_pending=false;
				}
				print_csharp_types(fptr, v_ptr->var_type);
				fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
				comma_is_pending=true;
			}
			v_ptr=v_ptr->prev;
		}

	}


	fprintf(fptr, "\t\t){\n");
	fprintf(fptr, "\t\t\tusing (SqlConnection cn = new SqlConnection(this.ConnectionString)){\n");
	fprintf(fptr, "\t\t\t\tSqlCommand cmd = new SqlCommand(\"srp_%s_Get%ss\", cn);\n",
					tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str()
			);
	fprintf(fptr, "\t\t\t\tcmd.CommandType = CommandType.StoredProcedure;\n");
	fprintf(fptr, "\t\t\t\tcmd.Parameters.Add(\"@PageIndex\", SqlDbType.Int).Value = pageIndex;\n");
	fprintf(fptr, "\t\t\t\tcmd.Parameters.Add(\"@PageSize\", SqlDbType.Int).Value = pageSize;\n");

	if(tableInfo_->has_search_key>0){
		struct var_list* v_ptr=tableInfo_->param_list;
		while(v_ptr){
			if(v_ptr->options.search_key){
				fprintf(fptr, "\t\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", v_ptr->var_name.c_str());
				print_csharp_db_types(fptr, v_ptr->var_type);
				fprintf(fptr, ").Value=");
				fprintf(fptr, " l_%s;\n", v_ptr->var_name.c_str());
			}
			v_ptr=v_ptr->prev;
		}
	}



	fprintf(fptr, "\t\t\t\tcn.Open();\n");
	fprintf(fptr, "\t\t\t\treturn Get%sCollectionFromReader(ExecuteReader(cmd));\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\t}\n");
	fprintf(fptr, "\t\t}\n");

}

void CSharpAspNetCodeGenerator::print_sql_provider_get_count (FILE * fptr){
	fprintf(fptr, "\tpublic override int Get%ssCount(\n", tableInfo_->tableName_.c_str());

	struct var_list* v_ptr=tableInfo_->param_list;
	bool comma_is_pending = false;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.search_key) {
			if(comma_is_pending){
				fprintf(fptr, ", ");
				comma_is_pending=false;
			}
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			comma_is_pending=true;
		}
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, ") {\n");
	fprintf(fptr, "\t\tusing (SqlConnection cn = new SqlConnection(this.ConnectionString)){\n");
	fprintf(fptr, "\t\t\tSqlCommand cmd = new SqlCommand(\"srp_%s_Get%ssCount\", cn);\n",
					tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\tcmd.CommandType = CommandType.StoredProcedure;\n");

	if(tableInfo_->has_search_key>0){
		v_ptr=tableInfo_->param_list;
		while(v_ptr){
			if(v_ptr->options.search_key){
				fprintf(fptr, "\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", v_ptr->var_name.c_str());
				print_csharp_db_types(fptr, v_ptr->var_type);
				fprintf(fptr, ").Value=");
				fprintf(fptr, " l_%s;\n", v_ptr->var_name.c_str());
			}
			v_ptr=v_ptr->prev;
		}
	}

	fprintf(fptr, "\t\t\tcn.Open();\n");
	fprintf(fptr, "\t\t\treturn (int)ExecuteScalar(cmd);\n");
	fprintf(fptr, "\t\t}\n");
	fprintf(fptr, "\t}\n\n");
}

void CSharpAspNetCodeGenerator::print_sql_provider_get_by_code(FILE * fptr){
	fprintf(fptr, "\tpublic override Biz%s Get%sByCode(", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str() );
	print_csharp_types(fptr, tableInfo_->param_list->var_type);
	fprintf(fptr, " ");
	tableInfo_->param_list->print_1st_var(fptr); fprintf(fptr, "){\n");
	fprintf(fptr, "\t\tusing (SqlConnection cn = new SqlConnection(this.ConnectionString)){\n");
	fprintf(fptr, "\t\t\tSqlCommand cmd = new SqlCommand(\"srp_%s_Get%sBy%s\", cn);\n",
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->param_list->var_name.c_str());
	fprintf(fptr, "\t\t\tcmd.CommandType = CommandType.StoredProcedure;\n");
	fprintf(fptr, "\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", tableInfo_->param_list->var_name.c_str());
	print_csharp_db_types(fptr, tableInfo_->param_list->var_type);
	fprintf(fptr, ").Value=");
	tableInfo_->param_list->print_1st_var(fptr);
	fprintf(fptr, ";\n");
	fprintf(fptr, "\t\t\tcn.Open();\n");
	fprintf(fptr, "\t\t\tIDataReader reader = ExecuteReader(cmd, CommandBehavior.SingleRow);\n");
	fprintf(fptr, "\t\t\tif (reader.Read())\n");
	fprintf(fptr, "\t\t\t\treturn Get%sFromReader(reader);\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\telse\n");
	fprintf(fptr, "\t\t\t\treturn null;\n");
	fprintf(fptr, "\t\t}\n");
	fprintf(fptr, "\t}\n");
}

void CSharpAspNetCodeGenerator::print_sql_provider_delete(FILE * fptr){

	fprintf(fptr, "\t/*\n");
	fprintf(fptr, "\tpublic override int Delete%s(int ", tableInfo_->tableName_.c_str());
	tableInfo_->param_list->print_1st_var(fptr); fprintf(fptr, "){\n");
	fprintf(fptr, "\t\tusing (SqlConnection cn = new SqlConnection(this.ConnectionString)){\n");
	fprintf(fptr, "\t\t\tSqlCommand cmd = new SqlCommand(\"srp_%s_Delete%s\", cn);\n",
				tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str()
			);
	fprintf(fptr, "\t\t\tcmd.CommandType = CommandType.StoredProcedure;\n");
	fprintf(fptr, "\t\t\tcmd.Parameters.Add(\"@%s\", SqlDbType.", tableInfo_->param_list->var_name.c_str());
	fprintf(fptr, ").Value =");
	tableInfo_->param_list->print_1st_var(fptr); fprintf(fptr, ";\n");
	fprintf(fptr, "\t\t\tcn.Open();\n");
	fprintf(fptr, "\t\t\tint ret = ExecuteNonQuery(cmd);\n");
	fprintf(fptr, "\t\t\treturn ret;\n");
	fprintf(fptr, "\t\t}\n");
	fprintf(fptr, "\t}\n");
	fprintf(fptr, "\t*/\n");
}

void CSharpAspNetCodeGenerator::print_sql_provider_insert(FILE * fptr){

	fprintf(fptr, "\tpublic override int Insert%s(Biz%s ", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	print_lower_fname(fptr);
	fprintf(fptr, "){\n");
	fprintf(fptr, "\t\tusing (SqlConnection cn = new SqlConnection(this.ConnectionString)){\n");
	fprintf(fptr, "\t\t\tSqlCommand cmd = new SqlCommand(\"srp_%s_Insert%s\", cn);\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\tcmd.CommandType = CommandType.StoredProcedure;\n");
	fprintf(fptr, "\n");
	print_insert_dp_params(fptr);
	fprintf(fptr, "\n");
	fprintf(fptr, "\t\t\tcn.Open();\n");
	fprintf(fptr, "\t\t\tint ret = ExecuteNonQuery(cmd);\n");
	fprintf(fptr, "\t\t\treturn (int)cmd.Parameters[\"@");
	fprintf(fptr, "%s", tableInfo_->param_list->var_name.c_str());
	fprintf(fptr, "\"].Value;\n");
	//fprintf(fptr, "\t\t\treturn ret;\n");
	fprintf(fptr, "\t\t}\n");
	fprintf(fptr, "\t}\n");
}

void CSharpAspNetCodeGenerator::print_sql_provider_update(FILE * fptr){
	fprintf(fptr, "\tpublic override int Update%s(Biz%s ", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	//tableInfo_->param_list->print_1st_var(fptr);
	print_lower_fname(fptr);
	fprintf(fptr, "){\n");
	fprintf(fptr, "\t\tusing (SqlConnection cn = new SqlConnection(this.ConnectionString)){\n");
	fprintf(fptr, "\t\t\tSqlCommand cmd = new SqlCommand(\"srp_%s_Update%s\", cn);\n",
					tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\tcmd.CommandType = CommandType.StoredProcedure;\n");
	print_dp_params(fptr);
	fprintf(fptr, "\n\n");
	fprintf(fptr, "\t\t\tcn.Open();\n");
	fprintf(fptr, "\t\t\tint ret = ExecuteNonQuery(cmd);\n");
	fprintf(fptr, "\t\t\treturn ret ;\n");
	fprintf(fptr, "\t\t}\n");
	fprintf(fptr, "\t}\n");
}

void CSharpAspNetCodeGenerator::print_bll_params(FILE* fptr){
	struct var_list* v_ptr=tableInfo_->param_list;
#define LARGE_BUFF 1024
	char buff1[LARGE_BUFF];
	char buff2[LARGE_BUFF];
	int nwrite1=0, nwrite2=0;

	while(v_ptr){
		char * bufptr1=buff1, *bufptr2=buff2;
		nwrite1=sprintf(bufptr1, "\t\tprivate ");
		nwrite2=sprintf(bufptr2, "\t\tpublic ");
		bufptr1 += nwrite1;
		bufptr2 += nwrite2;
		if(v_ptr->options.ref_table_name!=""){
			if(v_ptr->options.many==false){
				nwrite1=sprintf(bufptr1, "Biz%s", v_ptr->options.ref_table_name.c_str());
				nwrite2=sprintf(bufptr2, "Biz%s", v_ptr->options.ref_table_name.c_str());
			} else {
				nwrite1=sprintf(bufptr1, "List<Biz%s>", v_ptr->options.ref_table_name.c_str());
				nwrite2=sprintf(bufptr2, "List<Biz%s>", v_ptr->options.ref_table_name.c_str());
			}

		} else {
			switch (v_ptr->var_type){
				case INT32_TYPE:{
				nwrite1=sprintf(bufptr1, "int");
				nwrite2=sprintf(bufptr2, "int");

				}
				break;		
				case TEXT_TYPE:
				case VARCHAR_TYPE:
				case NVARCHAR_TYPE:
				case NCHAR_TYPE:
				case NTEXT_TYPE:
				nwrite1=sprintf(bufptr1, "string");
				nwrite2=sprintf(bufptr2, "string");

				
				break;		
				case FLOAT_TYPE:{
				nwrite1=sprintf(bufptr1, "float");
				nwrite2=sprintf(bufptr2, "float");

				}
				break;		
				case DOUBLE_TYPE:{
				nwrite1=sprintf(bufptr1, "double");
				nwrite2=sprintf(bufptr2, "double");

				}
				break;
				case BIT_TYPE:{
				nwrite1=sprintf(bufptr1, "bool");
				nwrite2=sprintf(bufptr2, "bool");

				}
				break;
				case DATETIME_TYPE:{
				nwrite1=sprintf(bufptr1, "DateTime");
				nwrite2=sprintf(bufptr2, "DateTime");

				}
				break;
				case BIGINT_TYPE:
				nwrite1=sprintf(bufptr1, "Int64");
				nwrite2=sprintf(bufptr2, "Int64");
				break;
				case TINYINT_TYPE:
				//fprintf(fptr, "byte");
				nwrite1=sprintf(bufptr1, "byte");
				nwrite2=sprintf(bufptr2, "byte");
				break;
				case IMAGE_TYPE:
				//fprintf(fptr, "Image");
				nwrite1=sprintf(bufptr1, "Image");
				nwrite2=sprintf(bufptr2, "Image");
				break;

				default:
				nwrite1=sprintf(bufptr1, "Unknown type : error ");
				nwrite2=sprintf(bufptr2, "Unknown type : error ");

			}
		}
		bufptr1 += nwrite1;
		bufptr2 += nwrite2;
			//nwrite1=sprintf(bufptr1, " <_%s>;\n", v_ptr->var_name.c_str());
		if(v_ptr->var_type==DATETIME_TYPE){
			nwrite1=sprintf(bufptr1, " _%s=DateTime.MinValue;\n", v_ptr->var_name.c_str());
			nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
					v_ptr->var_name.c_str(), 
					v_ptr->var_name.c_str(), 
					v_ptr->var_name.c_str());
		} else {
			if(v_ptr->options.ref_table_name!=""){
				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);
				nwrite1=sprintf(bufptr1, " _%s;\n", improved_name.c_str());
				nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
						improved_name.c_str(), 
						improved_name.c_str(), 
						improved_name.c_str());
			} else {
				nwrite1=sprintf(bufptr1, " _%s;\n", v_ptr->var_name.c_str());
				nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
						v_ptr->var_name.c_str(), 
						v_ptr->var_name.c_str(), 
						v_ptr->var_name.c_str());
			}
		}
		//nwrite2=sprintf(bufptr2, " %s{\n\t\t\tget{ return _%s;}\n\t\t\tset{ _%s=value;}\n\t\t}\n", 
		//		v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		fprintf(fptr, "%s", buff1);
		fprintf(fptr, "%s", buff2);
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "\t\tpublic ");
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " %s;\n", v_ptr->var_name.c_str());
		}

		v_ptr=v_ptr->prev;
	}
	v_ptr=tableInfo_->param_list;
	// print the properties of Fields which are inside a composite object
	// I need to modify the input language to accept UI flags but for now 
	// hard code everything
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			struct CSharpAspNetCodeGenerator* t_ptr = my_find_table( v_ptr->options.ref_table_name);
			struct var_list* vv_ptr = t_ptr->tableInfo_->param_list;
			while(vv_ptr){
				fprintf(fptr, "\t\tpublic ");
				print_csharp_types(fptr, vv_ptr->var_type);
				string orig_varname = v_ptr->var_name.c_str();
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos) ;
				//fprintf(fptr, " %s%s{\n", v_ptr->var_name.c_str(), vv_ptr->var_name.c_str());
				fprintf(fptr, " %s_%s{\n", improved_name.c_str(), vv_ptr->var_name.c_str());
				fprintf(fptr, "\t\t\tget{\n");
				fprintf(fptr, "\t\t\t\tif (%s !=null){\n", improved_name.c_str());
				fprintf(fptr, "\t\t\t\t\treturn %s.%s;\n", improved_name.c_str(),
						vv_ptr->var_name.c_str());
				fprintf(fptr, "\t\t\t\t} else \n");
				fprintf(fptr, "\t\t\t\t\treturn null;\n");
				fprintf(fptr, "\t\t\t\t}\n");
				fprintf(fptr, "\t\t\t}\n");
				vv_ptr=vv_ptr->prev;
			}
		}
		v_ptr=v_ptr->prev;
	}
}


void CSharpAspNetCodeGenerator:: print_sp(FILE * fptr){
	print_sp_insert(fptr);	
	print_sp_delete(fptr);	
	print_sp_update(fptr);	
	print_sp_select(fptr);	
	print_sp_count(fptr);	
	print_sp_select_by_pkey(fptr);	
	print_sp_unique_checks(fptr);
}



void CSharpAspNetCodeGenerator::print_sp_unique_checks(FILE* fptr){
	struct var_list* v_ptr = tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.unique){
			fprintf(fptr, "DROP PROCEDURE srp_%s_UniqueCheck_%s;\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "CREATE PROCEDURE srp_%s_UniqueCheck_%s(\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t@%s ", v_ptr->var_name.c_str());
			print_sp_types(fptr,  v_ptr->var_type);
			if(v_ptr->var_type==NVARCHAR_TYPE
				|| v_ptr->var_type==VARCHAR_TYPE 
				|| v_ptr->var_type==NCHAR_TYPE){
				fprintf(fptr, "(%d)\n", v_ptr->arr_len);
			} else fprintf(fptr, "\n");
			fprintf(fptr, ")\n");
			fprintf(fptr, "\nAS\nBEGIN\n\n\tSET NOCOUNT ON;\n");
			fprintf(fptr, "\tSELECT COUNT(*) FROM %s where %s like @%s;\n",
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str(),
				v_ptr->var_name.c_str());
			fprintf(fptr, "END\n");
			
		}

		v_ptr=v_ptr->prev;
	}
}

void CSharpAspNetCodeGenerator::print_sp_insert(FILE * fptr){
	print_sp_header(fptr);
	//fprintf(fptr, "CREATE PROCEDURE [dbo].[srp_%s_insert%s](\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "DROP PROCEDURE srp_%s_Insert%s\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "CREATE PROCEDURE srp_%s_Insert%s(\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());

	print_sp_params(fptr,  INSERT);
	fprintf(fptr, ")\n");


	// This code is based on that primary key is int
	if(tableInfo_->param_list){
		fprintf(fptr, "\nAS\nBEGIN\n\n\tSET NOCOUNT ON;\n");
		fprintf(fptr, "\tINSERT INTO %s (\n", tableInfo_->tableName_.c_str());
		struct var_list* v_ptr=tableInfo_->param_list;
		// Skip the 1st param - assume that it is the ouput parameter and print it out last
		v_ptr=v_ptr->prev;
		while(v_ptr){
			fprintf(fptr, "\t\t%s ", v_ptr->var_name.c_str());
			v_ptr=v_ptr->prev;
			if(v_ptr){
				fprintf(fptr, ",\n");
			}
			else fprintf(fptr, "\n");
		}
		fprintf(fptr, "\t\t) values (\n");
		v_ptr=tableInfo_->param_list;
		// Skip the 1st param - assume that it is the ouput parameter and print it out last
		v_ptr=v_ptr->prev;
		while(v_ptr){
			fprintf(fptr, "\t\t@%s", v_ptr->var_name.c_str());
			v_ptr=v_ptr->prev;
			if(v_ptr){
				fprintf(fptr, ",\n");
			}
		}
		fprintf(fptr, "\n\t)\n");
		fprintf(fptr, "\tSET @");
		//tableInfo_->param_list->print_1st_var(fptr); 
		fprintf(fptr, "%s", tableInfo_->param_list->var_name.c_str());
		fprintf(fptr, "= scope_identity()\n\nEND\n");
	}
}

void CSharpAspNetCodeGenerator::print_sp_update(FILE * fptr){
	print_sp_header(fptr);
	fprintf(fptr, "DROP PROCEDURE srp_%s_Update%s\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "CREATE PROCEDURE srp_%s_Update%s(\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());

	print_sp_params(fptr, UPDATE);
	fprintf(fptr, ")\n");
	fprintf(fptr, "\nAS\nBEGIN\n\n\tSET NOCOUNT ON;\n");
	fprintf(fptr, "\tUPDATE %s set \n", tableInfo_->tableName_.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	// for update skip the pkey - which we assume to be the first key
	v_ptr=v_ptr->prev;
	while(v_ptr){
		fprintf(fptr, "\t\t%s=@%s", v_ptr->var_name.c_str(), v_ptr->var_name.c_str() );
		v_ptr=v_ptr->prev;
		if(v_ptr){
			fprintf(fptr, ",\n");
		} else {
			fprintf(fptr, "\n");
		}
	}
	fprintf(fptr, "\tWHERE %s=@%s\nEND\n", tableInfo_->param_list->var_name.c_str(), tableInfo_->param_list->var_name.c_str() );

}

void CSharpAspNetCodeGenerator::print_sp_delete(FILE * fptr){
}

#include <sstream>
void CSharpAspNetCodeGenerator::print_sp_select(FILE * fptr){
	fflush(fptr);
	print_sp_header(fptr);
	fprintf(fptr, "DROP PROCEDURE srp_%s_Get%ss\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "CREATE PROCEDURE srp_%s_Get%ss(\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "@PageIndex  int,\n");
	fprintf(fptr, "@PageSize   int");
	// search key params
	

	if(tableInfo_->has_search_key){
		bool print_comma=true;
		print_sp_search_key_params(fptr,print_comma);
	}

	fprintf(fptr, ")\n");
	fprintf(fptr, "\nAS\nBEGIN\n\n\tSET NOCOUNT ON;\n");
	fprintf(fptr, "\tSELECT * FROM(\n");
	fprintf(fptr, "\t\tSELECT \n");
	fflush(fptr);
	struct var_list* v_ptr=tableInfo_->param_list;
	///stringstream select_clause, inner_join_clause, where_condition;
	print_sp_select_fields(fptr);
	fflush(fptr);
	fprintf(fptr, "\t\t\tROW_NUMBER() OVER (ORDER BY %s ) AS RowNum\n", tableInfo_->param_list->var_name.c_str());
	v_ptr=tableInfo_->param_list;
	int loop_counter=0;
	while(v_ptr){
		if(loop_counter==0)
			fprintf(fptr, "\t\tFROM %s\n", tableInfo_->tableName_.c_str());
		else if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos) ;

			fprintf(fptr, "\t\tINNER JOIN %s %s ON %s.%s=%s.%s \n",
				v_ptr->options.ref_table_name.c_str(),
				improved_name.c_str(),
				tableInfo_->tableName_.c_str(),
				v_ptr->var_name.c_str(),
				improved_name.c_str(),
				v_ptr->options.ref_field_name.c_str()
				);
		}
		v_ptr=v_ptr->prev;
		++loop_counter;
	}
	// print out search keys
	print_sp_search_key_whereclause(fptr);

	fprintf(fptr, "\t) srp_%s\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\tWHERE srp_%s.RowNum BETWEEN (@PageIndex*@PageSize+1) AND ((@PageIndex+1)*@PageSize)\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "END\n");
}


void CSharpAspNetCodeGenerator::print_sp_count(FILE* fptr){
	fprintf(fptr, "CREATE PROCEDURE srp_%s_Get%ssCount (\n ", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	if(tableInfo_->has_search_key){
		bool print_comma=false;
		cout << "print_sp_count:: passing print_comma=" 
			<< print_comma << "to func: print_sp_search_key_params"
			<< endl;
		print_sp_search_key_params(fptr,print_comma);
	}
	fprintf(fptr, ")\n");
	fprintf(fptr, "AS \nBEGIN\n");
	fprintf(fptr, "\tSELECT COUNT(*) as %s_Count from %s\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	print_sp_search_key_whereclause(fptr);
	fprintf(fptr, "\nEND\n");
}

void CSharpAspNetCodeGenerator::print_sp_select_by_pkey(FILE * fptr){
	print_sp_header(fptr);
	fprintf(fptr, "DROP PROCEDURE srp_%s_Get%sBy%s\n", 
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->param_list->var_name.c_str());
	fprintf(fptr, "CREATE PROCEDURE srp_%s_Get%sBy%s(\n", 
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->param_list->var_name.c_str());
	fprintf(fptr, "@%s  ", tableInfo_->param_list->var_name.c_str());
	print_sp_types(fptr, tableInfo_->param_list->var_type);
	if(tableInfo_->param_list->var_type==NVARCHAR_TYPE||tableInfo_->param_list->var_type==VARCHAR_TYPE ||
			tableInfo_->param_list->var_type==NCHAR_TYPE){
		fprintf(fptr, "(%d)\n", tableInfo_->param_list->arr_len);
	} else fprintf(fptr, "\n");
		
	fprintf(fptr, ")\n");
	fprintf(fptr, "\nAS\nBEGIN\n\n\tSET NOCOUNT ON;\n");
	fprintf(fptr, "\tSELECT \n");
	struct var_list* v_ptr=tableInfo_->param_list;
	print_sp_select_fields(fptr);
	v_ptr=tableInfo_->param_list;
	int loop_counter=0;
	while(v_ptr){
		if(loop_counter==0)
			fprintf(fptr, "\t\tFROM %s\n", tableInfo_->tableName_.c_str());
		else if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			fprintf(fptr, "\t\tINNER JOIN %s %s ON %s.%s=%s.%s \n",
				v_ptr->options.ref_table_name.c_str(),
				improved_name.c_str(),
				tableInfo_->tableName_.c_str(),
				v_ptr->var_name.c_str(),
				improved_name.c_str(),
				v_ptr->options.ref_field_name.c_str()
				);
		}
		v_ptr=v_ptr->prev;
		++loop_counter;
	}
	fprintf(fptr, "\twhere %s=@%s\n", tableInfo_->param_list->var_name.c_str(), 
			tableInfo_->param_list->var_name.c_str()  );

	fprintf(fptr, "\nEND\n"  );
}

void CSharpAspNetCodeGenerator:: print_sp_header(FILE * fptr){

	fprintf(fptr, "SET ANSI_NULLS ON\n");
	fprintf(fptr, "GO\n");
	fprintf(fptr, "SET QUOTED_IDENTIFIER ON\n");
	fprintf(fptr, "GO\n");

}

void CSharpAspNetCodeGenerator::print_bll(FILE * fptr){
	fprintf(fptr,"using System;\n");
	fprintf(fptr,"using System.Data;\n");
	fprintf(fptr,"using System.Configuration;\n");
	fprintf(fptr,"using System.Web;\n");
	fprintf(fptr,"using System.Web.Security;\n");
	fprintf(fptr,"using System.Web.UI;\n");
	fprintf(fptr,"using System.Web.UI.WebControls;\n");
	fprintf(fptr,"using System.Web.UI.WebControls.WebParts;\n");
	fprintf(fptr,"using System.Web.UI.HtmlControls;\n");
	fprintf(fptr,"using System.Collections.Generic;\n");
	fprintf(fptr,"using %s.DAL;\n", project_namespace);
	//fprintf(fptr,"using %s.BLL.%s;\n\n", project_namespace, rhs_name_space_name.c_str());
	fprintf(fptr,"\n");
	fprintf(fptr,"/// <summary>\n");
	fprintf(fptr,"/// Summary description for %s\n", tableInfo_->tableName_.c_str());
	fprintf(fptr,"/// </summary>\n");
	fprintf(fptr,"\n");
	fprintf(fptr,"\n");
	fprintf(fptr,"namespace %s.BLL.%s{\n", project_namespace, rhs_name_space_name.c_str());
	fprintf(fptr, "\tpublic class Biz%s: BizObject{\n", tableInfo_->tableName_.c_str());
	print_bll_params(fptr);
	print_bll_api(fptr);
	fprintf(fptr, "\t}\n");

	fprintf(fptr, "}\n");

}

void CSharpAspNetCodeGenerator::print_bll_api(FILE * fptr){

	fprintf(fptr,"\tprotected override bool CheckIfSubClassStateIsValid\n");
	fprintf(fptr,"\t{\n");
	fprintf(fptr,"\t\tget { return true; }\n");
	fprintf(fptr,"\t}\n");
	fprintf(fptr,"\n");
	fprintf(fptr, "\t/*\n");
	fprintf(fptr,"\tprotected static %sElement Settings\n", tableInfo_->tableName_.c_str());
	fprintf(fptr,"\t{\n");
	fprintf(fptr,"\t\tget { return Globals.Settings.%s; }\n", tableInfo_->tableName_.c_str());
	fprintf(fptr,"\t}\n");
	fprintf(fptr, "\t\n");
	fprintf(fptr, "\tprotected static CommonsElement Settings\n");
	fprintf(fptr, "\t{\n");
	fprintf(fptr, "\tget { return Globals.Settings.Commons; }\n");
	fprintf(fptr, "\t}\n");

	fprintf(fptr,"\t/// <summary>\n");
	fprintf(fptr,"\t/// Cache the input data, if caching is enabled\n");
	fprintf(fptr,"\t/// </summary>\n");
	fprintf(fptr,"\tprotected static void CacheData(string key, object data)\n");
	fprintf(fptr,"\t{\n");
	fprintf(fptr,"\t\tif (Settings.EnableCaching && data != null) {\n");
	fprintf(fptr,"\t\t\tBizObject.Cache.Insert(key, data, null,\n");
	fprintf(fptr,"\t\t\t\tDateTime.Now.AddSeconds(Settings.CacheDuration), TimeSpan.Zero);\n");
	fprintf(fptr,"\t\t}\n");
	fprintf(fptr,"\t}*/\n");
	fprintf(fptr,"\n");

	print_bll_constructors(fptr);
	print_bll_getcount(fptr);
	print_bll_get_collection(fptr);
	print_bll_get_paged_collection(fptr);
	print_bll_get_single_entity(fptr);
	print_bll_insert_entity(fptr);
	print_bll_update_entity(fptr);

}

void CSharpAspNetCodeGenerator::print_bll_constructors(FILE * fptr){
	print_bll_constructor(fptr);
	if(tableInfo_->has_composite_objs){	
		print_bll_bizobj_constructor(fptr);
	}
}

void CSharpAspNetCodeGenerator::print_bll_bizobj_constructor(FILE * fptr){
	fprintf(fptr, "\tpublic Biz%s(", tableInfo_->tableName_.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.ref_table_name==""){
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
		} else {
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			if(v_ptr->options.many){
				fprintf(fptr, "List<Biz%s> l_biz_%s", v_ptr->options.ref_table_name.c_str(), improved_name.c_str());
			} else {
				fprintf(fptr, "Biz%s l_biz_%s", v_ptr->options.ref_table_name.c_str(), improved_name.c_str());
			}
		}
		v_ptr=v_ptr->prev;
		if(v_ptr){
			fprintf(fptr, ", ");
		}
	}
	fprintf(fptr, "){\n");

	v_ptr=tableInfo_->param_list;
	// skip the primary key in the insert - it is added at the end as an output parameter
	// Also assume that primary key is always an integer
	while(v_ptr){
		if(v_ptr->options.ref_table_name!=""){
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			fprintf(fptr, "\t\tthis.%s= l_biz_%s;\n", improved_name.c_str(), improved_name.c_str());
		} else {
			fprintf(fptr, "\t\tthis.%s= l_%s;\n", v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "\t}\n");


}

void CSharpAspNetCodeGenerator::print_bll_constructor(FILE * fptr){
	fprintf(fptr, "\tpublic Biz%s(", tableInfo_->tableName_.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		print_csharp_types(fptr, v_ptr->var_type);
		fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;
		if(v_ptr){
			fprintf(fptr, ", ");
		}
	}
	fprintf(fptr, "){\n");

	v_ptr=tableInfo_->param_list;
	// skip the primary key in the insert - it is added at the end as an output parameter
	// Also assume that primary key is always an integer
	while(v_ptr){
		fprintf(fptr, "\t\tthis.%s= l_%s;\n", v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "\t}\n");


}

void CSharpAspNetCodeGenerator::print_bll_getcount(FILE* fptr){

	fprintf(fptr, "\t/// <summary>\n");
	fprintf(fptr, "\t/// Returns the number of total %s\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t/// </summary>\n");
	//fprintf(fptr, "\tpublic static int Get%ssCount()\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\tpublic static int Get%ssCount(", tableInfo_->tableName_.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	bool comma_is_pending = false;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.search_key) {
			if(comma_is_pending){
				fprintf(fptr, ", ");
				comma_is_pending=false;
			}
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			comma_is_pending=true;
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "){\n");

	
	fprintf(fptr, "\t{\n");
	fprintf(fptr, "\t\tint %sCount = 0;\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t/*\n");
	fprintf(fptr, "\t\tstring key = \"%s_%sCount\";\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\tif (Settings.EnableCaching  && BizObject.Cache[key] != null) {\n");
	fprintf(fptr, "\t\t\t%sCount = (int)BizObject.Cache[key];\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t} else {*/\n");
	//fprintf(fptr, "\t\t\t%sCount = SiteProvider.%s.Get%ssCount();\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\t%sCount = SiteProvider.%s.Get%ssCount(", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	comma_is_pending=false;
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.search_key) {
			if(comma_is_pending){
				fprintf(fptr, ", ");
				comma_is_pending=false;
			}
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			comma_is_pending=true;
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, ");\n");
	fprintf(fptr, "\t\t\t/*CacheData(key, %sCount);\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t}*/\n");
	fprintf(fptr, "\t\treturn %sCount;\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t}\n");

}


void CSharpAspNetCodeGenerator::print_bll_get_collection(FILE* fptr){

	fprintf(fptr, "\t/// <summary>\n");
	fprintf(fptr, "\t/// Returns a collection of total %ss\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t/// </summary>\n");
	fprintf(fptr, "\tpublic static List<Biz%s> Get%ss()\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t{\n");
	fprintf(fptr, "\t\treturn Get%ss(0, BizObject.MAXROWS);\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t}\n");

}

void CSharpAspNetCodeGenerator::print_bll_get_paged_collection(FILE* fptr){

	//fprintf(fptr, "\n\tpublic static List<Biz%s> Get%ss(int startRowIndex, int maximumRows) {\n", 
	//		tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	
	fprintf(fptr, "\n\tpublic static List<Biz%s> Get%ss(int startRowIndex, int maximumRows\n", 
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	bool comma_is_pending = false;
	if(tableInfo_->has_search_key>0){
		comma_is_pending=true;
	}
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.search_key) {
			if(comma_is_pending){
				fprintf(fptr, ", ");
				comma_is_pending=false;
			}
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			comma_is_pending=true;
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "){\n");



	fprintf(fptr, "\n");
	fprintf(fptr, "\t\tList<Biz%s> l_%s = new List<Biz%s>();\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t/*\n");
	fprintf(fptr, "\t\tstring key = \"%s_%s_\" + startRowIndex.ToString() + \"_\" + maximumRows.ToString();\n",
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\tif (Settings.EnableCaching && BizObject.Cache[key] != null) {\n");
	fprintf(fptr, "\t\t\tl_%s = (List<Biz%s>)BizObject.Cache[key];\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t} else {*/\n");
	fprintf(fptr, "\t\t\tl_%s = SiteProvider.%s.Get%ss(\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\t\tGetPageIndex(startRowIndex, maximumRows), maximumRows\n");

	comma_is_pending=false;
	if(tableInfo_->has_search_key>0){
		comma_is_pending=true;
	}
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		if(v_ptr->options.search_key) {
			if(comma_is_pending){
				fprintf(fptr, ", ");
				comma_is_pending=false;
			}
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			comma_is_pending=true;
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, ");\n");
	fprintf(fptr, "\t\t\t/*CacheData(key, l_%s);\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t}*/\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "\t\treturn l_%s;\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t}\n");

}

void CSharpAspNetCodeGenerator::print_bll_get_single_entity(FILE* fptr){

	fprintf(fptr,"\n\tpublic static Biz%s GetSingle%s(", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str() );
	/* primary key used here */
	print_csharp_types(fptr, tableInfo_->param_list->var_type);
	fprintf(fptr," pkey) {\n");
	fprintf(fptr,"\t\tBiz%s single_rec = SiteProvider.%s.Get%sByCode(pkey);\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(),
				tableInfo_->tableName_.c_str());
	fprintf(fptr,"\t\treturn single_rec;\n");
	fprintf(fptr,"\t}\n");

}


void CSharpAspNetCodeGenerator::print_bll_insert_entity(FILE * fptr){

	fprintf(fptr, "\t/// <summary>\n");
	fprintf(fptr, "\t/// Creates a new %s\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t/// </summary>\n");
	fprintf(fptr, "\tpublic static int Insert%s(", tableInfo_->tableName_.c_str());
	//print_func_params_without_pkey(fptr);
	print_func_params_with_pkey(fptr);
	fprintf(fptr, "){\n");
	// First convert nulls to empty strings
	struct var_list* v_ptr=tableInfo_->param_list;
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
	
	v_ptr=v_ptr->prev;
	fprintf(fptr, "\t\tbool fail_op=false;\n");
	while(v_ptr){
		if(v_ptr->options.unique){
			fprintf(fptr, "\t\tcount = SiteProvider.%s.UniqueCheck_%s(l_%s);\n",
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str(), 
				v_ptr->var_name.c_str());
			fprintf(fptr, "\t\tif(count>0){\n");
			fprintf(fptr, "\t\t\tthis.BrokenRules.Add(\"%s: \" + l_%s + \"already exists in Database\");\n",
				v_ptr->var_name.c_str(), v_ptr->var_name.c_str()
				);
			fprintf(fptr, "\t\t\tfail=true;\n");
			fprintf(fptr, "\t\t}\n");
		}
		
		if((v_ptr->var_type==NVARCHAR_TYPE || v_ptr->var_type==VARCHAR_TYPE 
			||v_ptr->var_type==TEXT_TYPE ||v_ptr->var_type==NTEXT_TYPE 
			||v_ptr->var_type==NCHAR_TYPE  )&&
				v_ptr->options.ref_table_name==""){
			if(v_ptr->options.null==false){
				fprintf(fptr, "\t\tif(l_%s==null){\n", v_ptr->var_name.c_str());
				fprintf(fptr, "\t\t\t// Put code to trap NOT NULL data\n");
				fprintf(fptr, "\t\t\treturn -1;\n");
				fprintf(fptr, "\t\t}\n");
			} else {
				fprintf(fptr, "\t\tl_%s=BizObject.ConvertNullToEmptyString(l_%s);\n", 
					v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
			}
		}
		v_ptr=v_ptr->prev;
	}
	// construct composite objects
	// no need for this actually

	//tableInfo_->param_list->print_1st_var(fptr);
	v_ptr=tableInfo_->param_list;
	fprintf(fptr, "\t\tl_");
	fprintf(fptr, "%s", v_ptr->var_name.c_str());
	fprintf(fptr, "=0;\n");
	fprintf(fptr, "\t\tBiz%s record = new Biz%s", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	print_bll_func_call_params(fptr);
	fprintf(fptr, ";\n");

	
	fprintf(fptr, "\t\tint ret = SiteProvider.%s.Insert%s(record);\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\tBizObject.PurgeCacheItems(\"%s\");\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\treturn ret;\n");
	fprintf(fptr, "\t}\n");
	fprintf(fptr, "\n");

}


void CSharpAspNetCodeGenerator::print_bll_update_entity(FILE* fptr){
	printf("came into print_bll_update_entity\n");

	fprintf(fptr, "\t/// <summary>\n");
	fprintf(fptr, "\t/// Updates a new %s\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t/// </summary>\n");
	fprintf(fptr, "\tpublic static int Update%s(", tableInfo_->tableName_.c_str());
	print_func_params_with_pkey(fptr);
	fprintf(fptr, "){\n");
	// First convert nulls to empty strings
	struct var_list* v_ptr=tableInfo_->param_list;
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
	while(v_ptr){
		if((v_ptr->var_type==NVARCHAR_TYPE || v_ptr->var_type==VARCHAR_TYPE 
			|| v_ptr->var_type==NCHAR_TYPE )&&
				v_ptr->options.ref_table_name==""){
			fprintf(fptr, "\t\tl_%s=BizObject.ConvertNullToEmptyString(l_%s);\n", v_ptr->var_name.c_str(),
				v_ptr->var_name.c_str()
				);
		}
		v_ptr=v_ptr->prev;
	}
	// construct composite objects
	fprintf(fptr, "\t\tBiz%s record = new Biz%s", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	//
	// local variable list of params
	//
	print_bll_func_call_params(fptr);
	fprintf(fptr, ";\n");
	fprintf(fptr, "\t\tint ret = SiteProvider.%s.Update%s(record);\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());

	fprintf(fptr, "\t\tBizObject.PurgeCacheItems(\"%s\");\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\treturn ret;\n");
	fprintf(fptr, "\t}\n");
	fprintf(fptr, "\n");
}


void CSharpAspNetCodeGenerator::print_func_params_without_pkey(FILE* fptr){
	// insert params come here
	struct var_list* v_ptr=tableInfo_->param_list;
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
	v_ptr=v_ptr->prev;
	while(v_ptr){
		fprintf(fptr, "\n\t\t");
		print_csharp_types(fptr, v_ptr->var_type);
		fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;
		if(v_ptr){
			fprintf(fptr, ", ");
		}
	}
}

void CSharpAspNetCodeGenerator::print_func_params_with_pkey(FILE* fptr){
	// insert params come here
	struct var_list* v_ptr=tableInfo_->param_list;
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
	bool outputted_something=false;
	while(v_ptr){
		if(v_ptr->options.ref_table_name=="" || v_ptr->options.many==false || v_ptr==tableInfo_->param_list /* 1st key = prim key is a foreign key reference*/){
			print_csharp_types(fptr, v_ptr->var_type);
			fprintf(fptr, " l_%s", v_ptr->var_name.c_str());
			outputted_something=true;
		} /*else if(v_ptr->options.many==false){
			fprintf(fptr, "Biz%s l_%s", v_ptr->options.ref_table_name.c_str(), v_ptr->options.ref_table_name.c_str());
			outputted_something=true;
		} */ else {
			outputted_something=false;
		}
		v_ptr=v_ptr->prev;
		if(v_ptr && outputted_something){
			fprintf(fptr, ", ");
			fprintf(fptr, "\n\t\t");
		}
	}

}

void CSharpAspNetCodeGenerator::print_sql_provider_header(FILE* fptr){

	fprintf(fptr, "\n");
	fprintf(fptr, "using System;\n");
	fprintf(fptr, "using System.Data;\n");
	fprintf(fptr, "using System.Data.SqlClient;\n");
	fprintf(fptr, "using System.Configuration;\n");
	fprintf(fptr, "using System.Web;\n");
	fprintf(fptr, "using System.Web.Security;\n");
	fprintf(fptr, "using System.Web.UI;\n");
	fprintf(fptr, "using System.Web.UI.WebControls;\n");
	fprintf(fptr, "using System.Web.UI.WebControls.WebParts;\n");
	fprintf(fptr, "using System.Web.UI.HtmlControls;\n");
	fprintf(fptr, "using System.Collections.Generic;\n");
	fprintf(fptr, "using %s.DAL.%s;\n", project_namespace, rhs_name_space_name.c_str());
	fprintf(fptr, "using %s.BLL.%s;\n", project_namespace, rhs_name_space_name.c_str());
	fprintf(fptr, "\n\n");
	fprintf(fptr, "/// <summary>\n");
	fprintf(fptr, "/// Summary description for Sql%sProvider\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "/// </summary>\n");
	fprintf(fptr, "namespace %s.DAL.SQLClient\n", project_namespace);
	fprintf(fptr, "{\n");
	fprintf(fptr, "\tpublic class Sql%sProvider : %sProvider{\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\n\n");
}


void CSharpAspNetCodeGenerator::print_aspx(FILE *fptr, bool called_recursively){
	if(called_recursively==false){
		print_aspx_header(fptr);
		bool ouput_script_manager=print_registered_controls(fptr);
		if(ouput_script_manager){
			fprintf(fptr, "\t<asp:ScriptManager id=\"ScriptManager1\" runat=\"server\"/>\n");
		}
	}
	print_aspx_objds_many(fptr);
	print_aspx_objds_single(fptr);

	if(tableInfo_->has_composite_objs){
		print_aspx_multi_view(fptr);
	}

	print_search_facility_aspx(fptr);


	print_aspx_formview(fptr);
	print_aspx_gridview(fptr);

	if(tableInfo_->has_multi){
		fprintf(fptr, "		</asp:Panel>\n");
		fprintf(fptr, " </asp:View>\n");
	}
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==true){
			struct CSharpAspNetCodeGenerator* t_ptr = my_find_table( v_ptr->options.ref_table_name);
			fprintf(fptr, "<asp:View ID=\"vw%s\" runat=\"server\">\n", v_ptr->options.ref_table_name.c_str());
			t_ptr->print_aspx(fptr, true);
			fprintf(fptr, "<asp:View>\n");
		}
		v_ptr=v_ptr->prev;
	}
	if(called_recursively==false){
		print_aspx_footer(fptr);
	}
	fprintf(fptr, "\n");
}

void CSharpAspNetCodeGenerator::print_aspx_params(FILE* fptr){
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\t\t\t<asp:Parameter Name=\"l_%s\" Type=\"", v_ptr->var_name.c_str()); 
		print_aspx_types(fptr, v_ptr->var_type);
		fprintf(fptr, "\" />\n");


		v_ptr=v_ptr->prev;
	}
}

void CSharpAspNetCodeGenerator::print_aspx_cs(FILE * fptr){
	print_aspx_cs_header(fptr);
	print_aspx_cs_body(fptr, false);
	print_aspx_cs_footer(fptr);
}

void CSharpAspNetCodeGenerator::print_aspx_cs_header(FILE* fptr){
	fprintf(fptr, "using System;\n");
	fprintf(fptr, "using System.Data;\n");
	fprintf(fptr, "using System.Configuration;\n");
	fprintf(fptr, "using System.Collections;\n");
	fprintf(fptr, "using System.Web;\n");
	fprintf(fptr, "using System.Web.Security;\n");
	fprintf(fptr, "using System.Web.UI;\n");
	fprintf(fptr, "using System.Web.UI.WebControls;\n");
	fprintf(fptr, "using System.Web.UI.WebControls.WebParts;\n");
	fprintf(fptr, "using System.Web.UI.HtmlControls;\n");
	fprintf(fptr, "using %s.UI.Controls;\n", project_namespace);
	fprintf(fptr, "using %s.BLL;\n", project_namespace);
	fprintf(fptr, "\n");
	fprintf(fptr, "namespace %s.UI\n", project_namespace);
	fprintf(fptr, "{\n");
	fprintf(fptr, "    public partial class Manage%s : BasePage\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "    {\n");
}

void CSharpAspNetCodeGenerator::print_aspx_cs_footer(FILE* fptr){
	fprintf(fptr, "\t}\n");
	fprintf(fptr, "}\n");
}

void CSharpAspNetCodeGenerator::print_aspx_cs_body(FILE *fptr, bool is_a_many_table){

	fprintf(fptr, "        protected void Page_Load(object sender, EventArgs e)\n");
	fprintf(fptr, "        {\n");
	fprintf(fptr, "\t\t\tif (gv%s.Rows.Count == 0) {\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\t\tfv%s.ChangeMode(FormViewMode.Insert);\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\t}\n");
	fprintf(fptr, "\t\t\t//else fv%s.ChangeMode(FormViewMode.Edit);\n", tableInfo_->tableName_.c_str());
	print_uc_event_registrations(fptr, tableInfo_->param_list);

	fprintf(fptr, "\n");
	fprintf(fptr, "        }\n");
	fprintf(fptr, " protected void gv%s_SelectedIndexChanged(object sender, EventArgs e)\n",
				tableInfo_->tableName_.c_str());
	fprintf(fptr, "        {\n");
	fprintf(fptr, "            fv%s.DataBind();\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "            GridView gv = (GridView)sender;\n");
	fprintf(fptr, "            GridViewRow gvr = gv.SelectedRow;\n");
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==true){
			fprintf(fptr, "            fv%s.Visible = true;\n", v_ptr->options.ref_table_name.c_str());
			fprintf(fptr, "            TextBox tb = (TextBox) fv%s.FindControl(\"tb_%s_Code\");\n",
						v_ptr->options.ref_field_name.c_str(), tableInfo_->tableName_.c_str());
			fprintf(fptr, "            if (tb != null)\n");
			fprintf(fptr, "            {\n");
			fprintf(fptr, "                tb.Text = gvr.Cells[0].Text;\n");
			fprintf(fptr, "            }\n");
			fprintf(fptr, "            fv%s.Caption = \"Details for %s: \" + gvr.Cells[1].Text;\n",
					v_ptr->options.ref_table_name.c_str() , v_ptr->options.ref_table_name.c_str());

		}
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, "        }\n");

	fprintf(fptr, "        protected void fv%s_ItemInserted(object sender, FormViewInsertedEventArgs e)\n",
		       	tableInfo_->tableName_.c_str());
	fprintf(fptr, "        {\n");
	fprintf(fptr, "            gv%s.DataBind();\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        }\n");

	fprintf(fptr, "\t\t\tprotected void fv%s_ItemUpdated(object sender, FormViewUpdatedEventArgs e)\n",
			tableInfo_->tableName_.c_str()
			);

	fprintf(fptr, "\t\t\t{\n");
	fprintf(fptr, "\t\t\tgv%s.DataBind();\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\t\t\t}\n");

	fprintf(fptr, "\t\t\tprotected void obj_%s_updating(object sender, ObjectDataSourceMethodEventArgs e){\n",
			tableInfo_->tableName_.c_str());

	fprintf(fptr, "\t\t\t\tIDictionary paramsFromPage = e.InputParameters;\n");
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "\t\t\t\tstring l_%s=\"\";\n", v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
	}
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!=""&&v_ptr->options.many==false){
			fprintf(fptr, "\t\t\t\tHiddenField hf_%s = (HiddenField) fv%s.FindControl(\"ehf_%s%s\");\n",
					v_ptr->var_name.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str()
					);
		} else {
			switch(v_ptr->var_type){
				case BIGINT_TYPE:{
				fprintf(fptr, "\t\t\t\tInt64 l_%s=Int64.Parse(paramsFromPage[\"%s\"].ToString());\n",
						v_ptr->var_name.c_str(), v_ptr->var_name.c_str()
						);
				}
				break;
				case INT32_TYPE:{
				fprintf(fptr, "\t\t\t\tint l_%s=Int32.Parse(paramsFromPage[\"%s\"].ToString());\n",
						v_ptr->var_name.c_str(), v_ptr->var_name.c_str()
						);
				}
				break;
				case TINYINT_TYPE:	{
				fprintf(fptr, "\t\t\t\tbyte l_%s=byte.Parse(paramsFromPage[\"%s\"].ToString());\n",
						v_ptr->var_name.c_str(), v_ptr->var_name.c_str()
						);
				}
				break;
				case BIT_TYPE:{
				fprintf(fptr, "\t\t\t\tbool l_%s=bool.Parse(paramsFromPage[\"%s\"].ToString());\n",
						v_ptr->var_name.c_str(), v_ptr->var_name.c_str()
						);
				}
				break;
				case DATETIME_TYPE:{
				fprintf(fptr, "\t\t\t\tDateTime l_%s=DateTime.Parse( paramsFromPage[\"%s\"].ToString());\n",
						v_ptr->var_name.c_str(), v_ptr->var_name.c_str()
						);
				}
				break;		
				/*
				case VARCHAR_TYPE:
				case NVARCHAR_TYPE:
				fprintf(fptr, "String");
				break;		
				case FLOAT_TYPE:{
				fprintf(fptr, "Float");
				}
				break;		
				case DOUBLE_TYPE:{
				fprintf(fptr, "Double");
				}
				break;
				case BIT_TYPE:{
				fprintf(fptr, "Bit");
				}
				break;
				*/
				default:
					fprintf(fptr, "\t\t\t\tstring l_%s=paramsFromPage[\"%s\"].ToString();\n",
							v_ptr->var_name.c_str(), v_ptr->var_name.c_str()
							);
			}
		}
		v_ptr=v_ptr->prev;
	}
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "\t\t\t\tif(hf_%s != null) {\n", v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t\t\t\tl_%s = hf_%s.Value;\n", v_ptr->var_name.c_str(),
					v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t\t\t}\n");
		}
		v_ptr=v_ptr->prev;
	}



        fprintf(fptr, "\t\t\t\tparamsFromPage.Clear();\n");
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\t\t\t\tparamsFromPage.Add(\"l_%s\", l_%s);\n", 
				v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, "\t\t}\n");
	fprintf(fptr, "        protected void obj_%s_inserting(object sender, ObjectDataSourceMethodEventArgs e)\n",
			tableInfo_->tableName_.c_str());
	fprintf(fptr, "        {\n");
	fprintf(fptr, "            IDictionary paramsFromPage = e.InputParameters;\n");
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "            HiddenField hf_%s = (HiddenField) fv%s.FindControl(\"ihf_%s%s\");\n",
				v_ptr->var_name.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str()
				);
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "\n");
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "           string l_%s=\"\";\n", v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, "\n");
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "           if(hf_%s != null) {\n", v_ptr->var_name.c_str());
			fprintf(fptr, "               l_%s = hf_%s.Value;\n", v_ptr->var_name.c_str(),
					v_ptr->var_name.c_str());
			fprintf(fptr, "           }\n");
		}
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, "            \n");
	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
		fprintf(fptr, "            paramsFromPage.Remove(\"l_%s\");\n", v_ptr->var_name.c_str());
		fprintf(fptr, "            paramsFromPage.Add(\"l_%s\", l_%s);\n", v_ptr->var_name.c_str(),
				v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "        }\n");
	


	print_uc_event_handlers(fptr, tableInfo_->param_list);

	v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==true){
			struct CSharpAspNetCodeGenerator* t_ptr = my_find_table( v_ptr->options.ref_table_name);
			if(t_ptr){
				fprintf(fptr, " /* printing code for MANY relationship table: %s*/\n", 
						v_ptr->options.ref_table_name.c_str());
				t_ptr->print_aspx_cs_body(fptr, true);
			}
			
		}
		v_ptr=v_ptr->prev;
	}

}



void CSharpAspNetCodeGenerator::print_sp_params(FILE * fptr, print_sp_params_mode mode){
	struct var_list * v_ptr=tableInfo_->param_list;
	if(mode==INSERT){
	// Skip the 1st param - assume that it is the ouput parameter and print it out last
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		fprintf(fptr, "\t@%s ", v_ptr->var_name.c_str());
		print_sp_types(fptr,  v_ptr->var_type);
		if(v_ptr->var_type==NVARCHAR_TYPE||v_ptr->var_type==VARCHAR_TYPE
			|| v_ptr->var_type==NCHAR_TYPE	){
			fprintf(fptr, "(%d)", v_ptr->arr_len);
		}
		v_ptr=v_ptr->prev;
		if(v_ptr){
			fprintf(fptr, ",\n");
		} else {
			fprintf(fptr, "\n");
		}
	}
	if(mode==INSERT){
		fprintf(fptr, "\t,@");
		fprintf(fptr, "%s ", tableInfo_->param_list->var_name.c_str()); 
		print_sp_types(fptr, tableInfo_->param_list->var_type);
		if(tableInfo_->param_list->var_type==NVARCHAR_TYPE||tableInfo_->param_list->var_type==VARCHAR_TYPE||
			tableInfo_->param_list->var_type==NCHAR_TYPE	){
			fprintf(fptr, "(%d)", tableInfo_->param_list->arr_len);
		}
		fprintf(fptr, " OUTPUT\n");
	}
}

static struct CSharpAspNetCodeGenerator* my_find_table( string ref_table_name){
	for(int i=0; i< table_info_table.size(); ++i){
		if(table_info_table[i]->tableInfo_->tableName_==ref_table_name){
			return table_info_table[i];
		}
	}
	return 0;
}

/*
void print_csharp_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case IMAGE_TYPE:
		fprintf(fptr, "Image");
		break;
		case TINYINT_TYPE:
		fprintf(fptr, "byte");
		break;
			
		case BIGINT_TYPE:{
		fprintf(fptr, "Int64");
		}
		break;		
		case INT32_TYPE:{
		fprintf(fptr, "int");
		}
		break;		
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
		case NTEXT_TYPE:
		case TEXT_TYPE:
			fprintf(fptr, "string");
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "float");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "bool");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "DateTime");
		}
		break;
		default:
			fprintf(fptr, "Error in parsing type file: %s, line:%d\n", __FILE__, __LINE__);
			fprintf(stderr, "case statement datatype not defined\nError in parsing type file: %s, line:%d\n", __FILE__, __LINE__);
	}
}
*/

/*
void print_sp_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case INT32_TYPE:{
		fprintf(fptr, "int");
		}
		break;		
		case NCHAR_TYPE:
		fprintf(fptr, "nchar");
		break;		
		case VARCHAR_TYPE:
		fprintf(fptr, "varchar");
		break;		
		case NVARCHAR_TYPE:
		fprintf(fptr, "nvarchar");
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "float");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "bit");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "datetime");
		}
		break;
		case TINYINT_TYPE:
		fprintf(fptr, "tinyint");
		break;
		case TEXT_TYPE:
		fprintf(fptr, "text");
		break;
		case NTEXT_TYPE:
		fprintf(fptr, "ntext");
		break;
		case BIGINT_TYPE:
		fprintf(fptr, "bigint");
		break;
		case IMAGE_TYPE:
		fprintf(fptr, "image");
		break;
		default:
		fprintf(fptr, "Invalid data type: file: %s, line: %d\n", __FILE__, __LINE__  );
	}
}
*/

/*
void print_csharp_db_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case IMAGE_TYPE:
		fprintf(fptr, "Image");
		break;	
		case INT32_TYPE:{
		fprintf(fptr, "Int");
		}
		break;		
		case VARCHAR_TYPE:{
		fprintf(fptr, "VarChar");
		}
		break;		
		case NVARCHAR_TYPE:{
		fprintf(fptr, "NVarChar");
		}
		break;		
		case NCHAR_TYPE:{
		fprintf(fptr, "NChar");
		}
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "Float");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "Double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "Bit");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "DateTime");
		}
		   break;
		case BIGINT_TYPE:
		fprintf(fptr, "BigInt");
		break;
		case NTEXT_TYPE:
		fprintf(fptr, "Ntext");
		break;
		case TEXT_TYPE:
		fprintf(fptr, "Text");
		break;
		case TINYINT_TYPE:
		fprintf(fptr, "TinyInt");
		break;
		default:
			fprintf(fptr, "Unknown Type: Error: File:%s, Line: %d\n", __FILE__, __LINE__);
	}
}
*/

/*
bool isOfStringType( datatype dt){
	if(dt==NVARCHAR_TYPE
		||dt==VARCHAR_TYPE 
		||dt==NTEXT_TYPE 
		||dt==TEXT_TYPE 
		||dt==NCHAR_TYPE){
		return true;
	} else 
		return false;
}
*/

/*
void print_aspx_types(FILE * fptr, datatype dt)
{
	switch (dt){
		case IMAGE_TYPE:
		fprintf(fptr, "Object");
		break;	
		case TINYINT_TYPE:
		fprintf(fptr, "Byte");
		break;		
		case BIGINT_TYPE:
		fprintf(fptr, "Int64");
		break;		
		
		case INT32_TYPE:{
		fprintf(fptr, "Int32");
		}
		break;		
		case NTEXT_TYPE:
		case TEXT_TYPE:
		case VARCHAR_TYPE:
		case NVARCHAR_TYPE:
		case NCHAR_TYPE:
		fprintf(fptr, "String");
		break;		
		case FLOAT_TYPE:{
		fprintf(fptr, "Single");
		}
		break;		
		case DOUBLE_TYPE:{
		fprintf(fptr, "Double");
		}
		break;
		case BIT_TYPE:{
		fprintf(fptr, "Boolean");
		}
		break;
		case DATETIME_TYPE:{
		fprintf(fptr, "DateTime");
		}
		break;
		default:
			fprintf(fptr, "Unknown Type: Error: File:%s, Line: %d\n", __FILE__, __LINE__);
	}
}
*/


void CSharpAspNetCodeGenerator::print_ascx(FILE* fptr){

	fprintf(fptr, "<%%@ Control Language=\"C#\" AutoEventWireup=\"true\" CodeFile=\"%sList.ascx.cs\" Inherits=\"%s.UI.Controls.%sList\" %%>\n",
		tableInfo_->tableName_.c_str(), project_namespace, tableInfo_->tableName_.c_str()	
		);
	fprintf(fptr, "<div id=\"%sList\" >\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "<br />                   \n");
	fprintf(fptr, "<br />\n");
	fprintf(fptr, "<asp:ObjectDataSource ID=\"ds%s\" runat=\"server\" SelectMethod=\"Get%ss\"\n",
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "    TypeName=\"%s.BLL.%s.Biz%s\" SelectCountMethod=\"Get%ssCount\">    \n",
			project_namespace, rhs_name_space_name.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "</asp:ObjectDataSource>\n");
	fprintf(fptr, "<asp:GridView ID=\"gv%s\" runat=\"server\" AllowPaging=\"True\" AutoGenerateColumns=\"False\" \n",
				tableInfo_->tableName_.c_str());
	fprintf(fptr, "    DataSourceID=\"ds%s\" OnSelectedIndexChanged=\"gv%s_SelectedIndexChanged\" >\n", 
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "    <Columns>\n");
	print_bound_fields(fptr, tableInfo_->param_list);
	fprintf(fptr, "    </Columns>\n");
	fprintf(fptr, "</asp:GridView>\n");
	fprintf(fptr, "<br />\n");
	fprintf(fptr, "</div>\n");

}

void CSharpAspNetCodeGenerator::print_bound_fields(FILE* fptr,struct var_list* start){
	struct var_list* v_ptr = start;
	while(v_ptr){
		fprintf(fptr, "            <asp:BoundField DataField=\"%s\" HeaderText=\"%s\" SortExpression=\"%s\" />\n",
				v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;

	}
	fprintf(fptr, "            <asp:CommandField ButtonType=\"Image\" ShowSelectButton=\"True\" SelectText=\"Edit\" SelectImageUrl=\"~/Images/Edit.gif\" />\n");
}

void CSharpAspNetCodeGenerator::print_ascx_cs(FILE* fptr){
	fprintf(fptr, "using System;\n");
	fprintf(fptr, "using System.Data;\n");
	fprintf(fptr, "using System.Configuration;\n");
	fprintf(fptr, "using System.Collections;\n");
	fprintf(fptr, "using System.Web;\n");
	fprintf(fptr, "using System.Web.Security;\n");
	fprintf(fptr, "using System.Web.UI;\n");
	fprintf(fptr, "using System.Web.UI.WebControls;\n");
	fprintf(fptr, "using System.Web.UI.WebControls.WebParts;\n");
	fprintf(fptr, "using System.Web.UI.HtmlControls;\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "namespace %s.UI.Controls\n", project_namespace);
	fprintf(fptr, "{\n");
	fprintf(fptr, "    \n");
	fprintf(fptr, "    public partial class %sList : System.Web.UI.UserControl\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "    {\n");
	fprintf(fptr, "        public delegate void %sCodeSelectEventHandler(object sender, EventArgs e);\n",
					tableInfo_->tableName_.c_str());

	fprintf(fptr, "\n");
	fprintf(fptr, "        public event %sCodeSelectEventHandler %sCodeSelectCommand;\n", 
				tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\n");
	fprintf(fptr, "                \n");
	fprintf(fptr, "        protected void Page_Load(object sender, EventArgs e)\n");
	fprintf(fptr, "        {\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "        }\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "        protected void  gv%s_SelectedIndexChanged(object sender, EventArgs e)\n",
				tableInfo_->tableName_.c_str());
	fprintf(fptr, "        {\n");
	fprintf(fptr, "            //RaiseEvent AccountSelectCommand(Me, objAccount.AccountId)\n");
	fprintf(fptr, "            %sCodeSelectCommand(sender, e);\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "            \n");
	fprintf(fptr, "        }\n");
	fprintf(fptr, "	    }\n");
	fprintf(fptr, "}\n");
}

bool CSharpAspNetCodeGenerator::print_registered_controls(FILE* fptr){
	struct var_list* v_ptr=tableInfo_->param_list;
	bool output_script_manager=false;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!=""&& v_ptr->options.many==false){
			fprintf(fptr,"<%%@ Register Src=\"../Controls/%sList.ascx\" TagName=\"%sList\" TagPrefix=\"sr\" %%>\n",
				v_ptr->options.ref_table_name.c_str(), v_ptr->options.ref_table_name.c_str());
			output_script_manager=true;
		}
		v_ptr=v_ptr->prev;
	}
	return output_script_manager;
}

void CSharpAspNetCodeGenerator::print_aspx_edit_fields(FILE* fptr, struct var_list* param_list)
{
	struct var_list* v_ptr=param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name==""||v_ptr==param_list /* what this really means is if
			   the field is the primary key which we assume to be the 1st key*/
								   ){
			fprintf(fptr, "\t\t<tr><td>%s:</td>\n", v_ptr->var_name.c_str());
			if(v_ptr->var_type==BIT_TYPE){
				fprintf(fptr, "\t\t<td><asp:CheckBox ID=\"cb_%s\" runat=\"server\" Checked='<%%# Bind(\"%s\") %%>' />\n",
				v_ptr->var_name.c_str(),  v_ptr->var_name.c_str() );
			} else {

				fprintf(fptr, "\t\t<td><asp:TextBox ID=\"tb_%s\" runat=\"server\" Text='<%%# Bind(\"",
				v_ptr->var_name.c_str()   );
			fprintf(fptr, "%s\") %%>'/>\n",  v_ptr->var_name.c_str()  );
			}
			fprintf(fptr, "\t\t</td></tr>\n");
		} else {
			fprintf(fptr, "\t\t<tr><td>\n");
			fprintf(fptr, "\t\t<asp:UpdatePanel id=\"eup_%s%s\" runat=\"server\">\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t<contenttemplate>\n");
			fprintf(fptr, "\t\t<table>\n");

			fprintf(fptr, "\t\t<tr><td>%s: </td>\n\t\t<td> <asp:TextBox ID=\"etb_%s%s\" runat=\"server\" Text='<%%# Bind(\"",
				v_ptr->var_name.c_str(),  tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			fprintf(fptr, "%s\") %%>'/></td>\n",  v_ptr->var_name.c_str()  );
			fprintf(fptr, "\t\t<td> <asp:HiddenField ID=\"ehf_%s%s\" runat=\"server\" Value='<%%# Bind(\"",
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str()  );
			fprintf(fptr, "%s\") %%>'/>\n",  v_ptr->var_name.c_str()  );

			fprintf(fptr, "\t\t<asp:Button id=\"ebtshowgv%s%s\" onclick=\"ebtshowgv%s%s_Click\" runat=\"server\" Text=\"...\"></asp:Button>\n", 
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str(), tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t</td></tr><tr><td colspan=\"3\">\n");
			fprintf(fptr, "\t\t<sr:%sList id=\"euc%s%sList\" runat=\"server\" Visible=\"False\" />\n", 
					v_ptr->options.ref_table_name.c_str(), tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			fprintf(fptr, "\t\t</td></tr>\n"); 
			fprintf(fptr, "\t\t</table>\n");
			fprintf(fptr, "\t\t</contenttemplate>\n");
			fprintf(fptr, "\t\t</asp:UpdatePanel>\n");
			fprintf(fptr, "\t\t</td></tr>\n");
			
		}
		v_ptr=v_ptr->prev;
	}
}

void CSharpAspNetCodeGenerator::print_uc_event_registrations(FILE* fptr, struct var_list* param_list)
{
	struct var_list* v_ptr=param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "\t\t\tif (iuc%s%sList !=null)\n", tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t\t\tiuc%s%sList.%sCodeSelectCommand += \n\t\t\t new %sList.%sCodeSelectEventHandler(this.i%s%sSelectClick);\n",
			tableInfo_->tableName_.c_str(),		
			v_ptr->var_name.c_str(), v_ptr->options.ref_table_name.c_str(),
			v_ptr->options.ref_table_name.c_str(), v_ptr->options.ref_table_name.c_str(),
			tableInfo_->tableName_.c_str(),
			v_ptr->var_name.c_str()
			);
			fprintf(fptr, "\t\t\tif (euc%s%sList !=null)\n", tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t\t\teuc%s%sList.%sCodeSelectCommand += \n\t\t\t new %sList.%sCodeSelectEventHandler(this.e%s%sSelectClick);\n",
			tableInfo_->tableName_.c_str(),		
			v_ptr->var_name.c_str(), v_ptr->options.ref_table_name.c_str(),
			v_ptr->options.ref_table_name.c_str(), v_ptr->options.ref_table_name.c_str(),
			tableInfo_->tableName_.c_str(),
			v_ptr->var_name.c_str()
			);
				
		}
		v_ptr=v_ptr->prev;
	}
}

void CSharpAspNetCodeGenerator::print_uc_event_handlers(FILE* fptr, struct var_list* param_list){
	struct var_list* v_ptr=param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "\tprotected void e%s%sSelectClick(object sender, EventArgs e){\n",
				tableInfo_->tableName_.c_str(),	v_ptr->var_name.c_str());
			fprintf(fptr, "\t\tGridView mygrid=(GridView)sender;\n");
			fprintf(fptr, "\t\tGridViewRow myrow=mygrid.SelectedRow;\n");
			fprintf(fptr, "\t\tetb_%s%s.Text = myrow.Cells[1].Text;\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			fprintf(fptr, "\t\tehf_%s%s.Value = myrow.Cells[0].Text;\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			fprintf(fptr, "\t}\n");
			fprintf(fptr, "\tprotected void i%s%sSelectClick(object sender, EventArgs e){\n",
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\tGridView mygrid=(GridView)sender;\n");
			fprintf(fptr, "\t\tGridViewRow myrow=mygrid.SelectedRow;\n");
			fprintf(fptr, "\t\titb_%s%s.Text = myrow.Cells[1].Text;\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			fprintf(fptr, "\t\tihf_%s%s.Value = myrow.Cells[0].Text;\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			fprintf(fptr, "\t}\n");

			// now ouput the expose grid button event handler
			// one for the insert template
			fprintf(fptr, "\tprotected void ibtshowgv%s%s_Click(object sender, EventArgs e){\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\tiuc%s%sList.Visible=!iuc%s%sList.Visible;\n\t}\n",
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str(), 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str()); 
			// one for the edit template
			fprintf(fptr, "\tprotected void ebtshowgv%s%s_Click(object sender, EventArgs e){\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\teuc%s%sList.Visible=!euc%s%sList.Visible;\n\t}\n",
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str(), 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str()); 



		}
		v_ptr=v_ptr->prev;
	}
}


void CSharpAspNetCodeGenerator::print_aspx_insert_fields(FILE* fptr, struct var_list* param_list){
	struct var_list* v_ptr=param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name==""|| v_ptr==tableInfo_->param_list
				/* what this means is we are at the primary key*/){
			fprintf(fptr, "\t\t<tr><td>%s:</td>\n", v_ptr->var_name.c_str());
			if(v_ptr->var_type==BIT_TYPE){
				fprintf(fptr, "\t\t<td><asp:CheckBox ID=\"cb_%s\" runat=\"server\" Checked='<%%# Bind(\"l_%s\") %%>' />\n",
				v_ptr->var_name.c_str(),  v_ptr->var_name.c_str() );
			} else {

				fprintf(fptr, "\t\t<td><asp:TextBox ID=\"tb_%s\" runat=\"server\" Text='<%%# Bind(\"l_",
				v_ptr->var_name.c_str()   );
				fprintf(fptr, "%s\") %%>'/>\n",  v_ptr->var_name.c_str()  );
				print_aspx_validator(fptr, v_ptr);
			}
			fprintf(fptr, "\t\t</td></tr>\n");
		} else if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			fprintf(fptr, "\t\t<tr><td>\n");
			fprintf(fptr, "\t\t<asp:UpdatePanel id=\"iup_%s%s\" runat=\"server\">\n", 
					tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t<contenttemplate>\n");
			fprintf(fptr, "\t\t<table>\n");

			fprintf(fptr, "\t\t<tr><td>%s: </td> <td> <asp:TextBox ID=\"itb_%s%s\" runat=\"server\" Text='<%%# Bind(\"l_",
				v_ptr->var_name.c_str(),  tableInfo_->tableName_.c_str(),v_ptr->var_name.c_str() );
			fprintf(fptr, "%s\") %%>'/>\n",  v_ptr->var_name.c_str()  );
			fprintf(fptr, "\t\t<td> <asp:HiddenField ID=\"ihf_%s%s\" runat=\"server\" Value='<%%# Bind(\"l_",
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str()  );
			fprintf(fptr, "%s\") %%>'/>\n",  v_ptr->var_name.c_str()  );

			fprintf(fptr, "\t\t<asp:Button id=\"btshowgv%s%s\" onclick=\"ibtshowgv%s%s_Click\" runat=\"server\" Text=\"...\"></asp:Button>\n", 
				tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str(), tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			fprintf(fptr, "\t\t</td></tr><tr><td colspan=\"3\">\n");
			fprintf(fptr, "\t\t<sr:%sList id=\"iuc%s%sList\" runat=\"server\" Visible=\"False\" />\n", 
					v_ptr->options.ref_table_name.c_str(), tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			fprintf(fptr, "\t\t</td></tr>\n"); 
			fprintf(fptr, "\t\t</table>\n");
			fprintf(fptr, "\t\t</contenttemplate>\n");
			fprintf(fptr, "\t\t</asp:UpdatePanel>\n");
			fprintf(fptr, "\t\t</tr></td>\n");
			
		}
		v_ptr=v_ptr->prev;
	}
}

void CSharpAspNetCodeGenerator::print_sp_select_params(FILE* fptr, bool with_pkey, bool rename_vars, string inner_join_tabname){
	fprintf(fptr, "/*Entering print_sp_select_params called with params: %d %d %s */\n",
			with_pkey, rename_vars, inner_join_tabname.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	if(!with_pkey){
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			CSharpAspNetCodeGenerator* tbl_ptr=my_find_table(v_ptr->options.ref_table_name);
			if(tbl_ptr){
				tbl_ptr->print_sp_select_params(fptr, false, true, v_ptr->options.ref_table_name.c_str());
			} else {
				fprintf(fptr, "referenced table: %s not found in table list: line no: %d",
						v_ptr->options.ref_table_name.c_str(), line_no);
				fprintf(fptr, "referenced table: %s not found in table list: line no: %d ... exiting",
						v_ptr->options.ref_table_name.c_str(), line_no);
				exit(1);
				
			}
			if(tbl_ptr){
				fprintf(fptr, ",");
			}
			fprintf(fptr, "\t\t\t%s.%s,\n", tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
			//print_sp_select_params(fptr, with_pkey, rename_vars, v_ptr->var_name.c_str());
		} else {
			if(rename_vars){
				string orig_varname = inner_join_tabname;
				int pos = orig_varname.find("_Code");
				string improved_name = orig_varname.substr(0, pos);

				fprintf(fptr, "\t\t\t%s.%s as %s_%s", improved_name.c_str(), v_ptr->var_name.c_str(),
						improved_name.c_str(), v_ptr->var_name.c_str());
			} else {
				fprintf(fptr, "\t\t\t%s.%s", tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
			}
		}
		
		v_ptr=v_ptr->prev;
		if(v_ptr){
			fprintf(fptr, ",\n ");
		} else 
			fprintf(fptr, "\n ");
	}
	fprintf(fptr, "/*Exiting print_sp_select_params called with params: %d %d %s */\n",
			with_pkey, rename_vars, inner_join_tabname.c_str());
}

void CSharpAspNetCodeGenerator::print_reader(FILE * edit_out, bool with_pkey, bool rename_vars, string inner_join_tabname)
{
	struct var_list * v_ptr=tableInfo_->param_list;
	if(!with_pkey){
		v_ptr=v_ptr->prev;
	}
	while(v_ptr){
		if(v_ptr->options.many==false){
			char buffer[MAX_VAR_LEN];
			if(v_ptr->options.ref_table_name!=""){
				//sprintf(buffer, "%s", v_ptr->var_name.c_str());
				struct CSharpAspNetCodeGenerator* t_ptr = my_find_table( v_ptr->options.ref_table_name);
				if(t_ptr==0){
					fprintf(edit_out, "table: %s not found: line: %d, file: %s\n",
							v_ptr->options.ref_table_name.c_str(), __LINE__, __FILE__);
				} else {
					//t_ptr->print_reader(edit_out, false, true, v_ptr->var_name);
					//fprintf(edit_out, ",\n");
					string orig_varname = v_ptr->var_name.c_str();
					int pos = orig_varname.find("_Code");
					string improved_name = orig_varname.substr(0, pos);
					sprintf(buffer, "\t\t\tl_biz_%s", improved_name.c_str());
					fprintf(edit_out, "%s", buffer);
				}

			} else {
				if(rename_vars){
					//fprintf(edit_out, "renaming var: rename_vars: %d inner_join_tabname: %s\n", 
					//		rename_vars, inner_join_tabname.c_str());
					string orig_varname = inner_join_tabname;
					int pos = orig_varname.find("_Code");
					string improved_name = orig_varname.substr(0, pos);
					sprintf(buffer, "%s_%s", improved_name.c_str(),v_ptr->var_name.c_str());
				}else {
					sprintf(buffer, "%s", v_ptr->var_name.c_str());
				}
				switch (v_ptr->var_type){
					case IMAGE_TYPE:
					fprintf(edit_out, "\t\t\t(Image) reader[\"%s\"]", buffer);
					break;
					case TINYINT_TYPE:
					fprintf(edit_out, "\t\t\t(byte) reader[\"%s\"]", buffer);
					break;
					case BIGINT_TYPE:
					fprintf(edit_out, "\t\t\t(Int64) reader[\"%s\"]", buffer);
					break;
					case INT32_TYPE:{
					fprintf(edit_out, "\t\t\t(int) reader[\"%s\"]", buffer);
					}
					break;		
					case BIT_TYPE:{
					fprintf(edit_out, "\t\t\t(bool) reader[\"%s\"]", buffer);
					}
					break;
					case DATETIME_TYPE:{
					fprintf(edit_out, "\t\t\t(DateTime) reader[\"%s\"]", buffer);
					}
					break;
					case NTEXT_TYPE:
					case TEXT_TYPE:
					case VARCHAR_TYPE:
					case NVARCHAR_TYPE:
					case NCHAR_TYPE:
					fprintf(edit_out, "\t\t\treader[\"%s\"].ToString()", buffer);
					
					break;		
					case FLOAT_TYPE:{
					fprintf(edit_out, "\t\t\tConvert.ToSingle(  reader[\"%s\"].ToString())", 
						buffer);
					}
					break;		
					case DOUBLE_TYPE:{
					fprintf(edit_out, "\t\t\tConvert.ToDouble( reader[\"%s\"].ToString())", 
						buffer);
					}
					break;
					default:
					fprintf(edit_out, "\t\t\tUnknown Type: FILE:%s, line:%d, function: print_reader(fptr)\n",
							__FILE__, __LINE__);
				}
					

			}

		} else {
			// the list would have been constructed - just pass it to the constructor
			fprintf(edit_out, "\t\t\tl_biz_%s", v_ptr->var_name.c_str());
		}
		v_ptr=v_ptr->prev;
		if(v_ptr) {
			fprintf(edit_out, ",\n");
		}
	}
	//if(rename_vars==true)
	//	fprintf(edit_out, "\n\t\t\t/* exiting ... print_reader called with rename_vars=true*/\n");
}

	void CSharpAspNetCodeGenerator::print_reader_param_with_cast(FILE* fptr, var_list* v_ptr){
		// this first variable 
		char buffer[MAX_VAR_LEN];
		if(v_ptr->options.ref_table_name==""){
			sprintf(buffer, "%s", v_ptr->var_name.c_str());
		} else {
			string orig_varname = v_ptr->var_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			sprintf(buffer, "%s_Code", improved_name.c_str());
		}
		switch (v_ptr->var_type){
			case IMAGE_TYPE:
			fprintf(fptr, "\t\t\t(Image) reader[\"%s\"]", buffer);
			break;
			case TINYINT_TYPE:
			fprintf(fptr, "\t\t\t(byte) reader[\"%s\"]", buffer);
			break;
			case BIGINT_TYPE:
			fprintf(fptr, "\t\t\t(Int64) reader[\"%s\"]", buffer);
			break;
			case INT32_TYPE:{
			fprintf(fptr, "\t\t\t(int) reader[\"%s\"]", buffer);
			}
			break;		
			case BIT_TYPE:{
			fprintf(fptr, "\t\t\t(bool) reader[\"%s\"]", buffer);
			}
			break;
			case DATETIME_TYPE:{
			fprintf(fptr, "\t\t\t(DateTime) reader[\"%s\"]", buffer);
			}
			break;
			case NTEXT_TYPE:
			case TEXT_TYPE:
			case VARCHAR_TYPE:
			case NVARCHAR_TYPE:
			case NCHAR_TYPE:
			fprintf(fptr, "\t\t\treader[\"%s\"].ToString()", buffer);
			
			break;		
			case FLOAT_TYPE:{
			fprintf(fptr, "\t\t\tConvert.ToSingle(  reader[\"%s\"].ToString())", 
				buffer);
			}
			break;		
			case DOUBLE_TYPE:{
			fprintf(fptr, "\t\t\tConvert.ToDouble( reader[\"%s\"].ToString())", 
				buffer);
			}
			break;
			default:
			fprintf(fptr, "\t\t\tUnknown Type: FILE:%s, line:%d, function: print_reader(fptr)\n",
					__FILE__, __LINE__);
		}
	}

	void CSharpAspNetCodeGenerator::print_bll_func_call_params(FILE* fptr){
		struct var_list* v_ptr=tableInfo_->param_list;
		fprintf(fptr, "(\n");
		bool printed=false;
		while(v_ptr){
			if(v_ptr->options.ref_table_name==""||v_ptr==tableInfo_->param_list
				|| (v_ptr->options.ref_table_name!="" && v_ptr->options.many==false)){
				fprintf(fptr,"\t\t\tl_%s" , v_ptr->var_name.c_str());
				printed = true;
			} 
			
			v_ptr=v_ptr->prev;
			if(v_ptr && printed) {
				fprintf(fptr, ",\n");
			}
		}
		fprintf(fptr, ")");

	}


void CSharpAspNetCodeGenerator::print_sp_select_fields(FILE *fptr){
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.ref_table_name!="" && v_ptr->options.many==false){
			CSharpAspNetCodeGenerator* tbl_ptr=my_find_table(v_ptr->options.ref_table_name);
			if(tbl_ptr){
				tbl_ptr->print_sp_select_params(fptr, false, true, v_ptr->var_name.c_str());
			} else {
				fprintf(fptr, "referenced table: %s not found in table list: line no: %d",
						v_ptr->options.ref_table_name.c_str(), line_no);
				fprintf(fptr, "referenced table: %s not found in table list: line no: %d ... exiting",
						v_ptr->options.ref_table_name.c_str(), line_no);
				exit(1);
				
			}
			if(tbl_ptr){
				fprintf(fptr, ",");
			}
			fprintf(fptr, "\t\t\t%s.%s,\n", tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str() );
		} else {
			fprintf(fptr, "\t\t\t%s,\n", v_ptr->var_name.c_str() );
			fflush(fptr);
		}
		v_ptr=v_ptr->prev;
	}
}

void CSharpAspNetCodeGenerator::print_aspx_multi_view(FILE *fptr){
	fprintf(fptr, "<asp:MultiView ID=\"mv%s\" runat=\"server\" ActiveViewIndex=\"0\">\n",
		tableInfo_->tableName_.c_str()	);
	fprintf(fptr, "        <asp:View ID=\"vw%s\" runat=\"server\">\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "            <asp:Panel ID=\"pn_nav_%s\" runat=\"server\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "                 CssClass=\"TabContainer\">\n");
	fprintf(fptr, "                <asp:Label ID=\"lb_nav_%s\" runat=\"server\" Text=\"%s\" CssClass=\"TabItemActive\" ></asp:Label>\n",
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		if(v_ptr->options.many){
			string orig_varname = v_ptr->options.ref_table_name.c_str();
			int pos = orig_varname.find("_Code");
			string improved_name = orig_varname.substr(0, pos);
			fprintf(fptr, "                <asp:LinkButton ID=\"btnNextView\" runat=\"server\" CommandName=\"NextView\" Text=\"Edit %s\" \n", improved_name.c_str());
			fprintf(fptr, "                    CssClass=\"TabItemInActive\" Visible=\"false\"\n");
			fprintf(fptr, "                />\n");
			
		}
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, "            </asp:Panel>  \n");
	fprintf(fptr, "	<asp:Panel ID=\"pn_%s\" runat=\"server\"  CssClass=\"ContentPanel\" >\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "\n");
	
}

void CSharpAspNetCodeGenerator::print_aspx_objds_many(FILE* fptr){
	fprintf(fptr, "    <asp:ObjectDataSource ID=\"ds%s\" runat=\"server\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        InsertMethod=\"Insert%s\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        SelectMethod=\"Get%ss\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        SelectCountMethod=\"Get%ssCount\"\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        EnablePaging=\"True\"  \n");
	fprintf(fptr, "        TypeName=\"%s.BLL.%s.Biz%s\" UpdateMethod=\"Update%s\" \n",
				project_namespace, rhs_name_space_name.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "        >\n");
	fprintf(fptr, "        <InsertParameters>\n");
	print_aspx_params(fptr);
	fprintf(fptr, "        </InsertParameters>\n");
	fprintf(fptr, "        <UpdateParameters>\n");
	print_aspx_params(fptr);
	fprintf(fptr, "        </UpdateParameters>\n");
	fprintf(fptr, "    </asp:ObjectDataSource>\n");
	fprintf(fptr, "    \n");
}

void CSharpAspNetCodeGenerator::print_aspx_objds_single(FILE* fptr){
	fprintf(fptr, "    <asp:ObjectDataSource ID=\"dsSingle%s\" runat=\"server\"\n", 
			tableInfo_->tableName_.c_str());
	fprintf(fptr, "        SelectMethod=\"GetSingle%s\"\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        InsertMethod=\"Insert%s\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        UpdateMethod=\"Update%s\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        OnUpdating=\"obj_%s_updating\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        OnInserting=\"obj_%s_inserting\" \n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "        TypeName=\"%s.BLL.%s.Biz%s\">\n", 
			project_namespace, rhs_name_space_name.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "        <SelectParameters>\n");
	fprintf(fptr, "            <asp:ControlParameter ControlID=\"gv%s\" DefaultValue=\"1\" Name=\"pkey\"\n", 
			tableInfo_->tableName_.c_str());
	fprintf(fptr, "                PropertyName=\"SelectedValue\" Type=\"Int32\" />\n");
	fprintf(fptr, "        </SelectParameters>\n");
	fprintf(fptr, "        <InsertParameters>\n");
	print_aspx_params(fptr);
	fprintf(fptr, "        </InsertParameters>\n");
	fprintf(fptr, "        <UpdateParameters>\n");
	print_aspx_params(fptr);
	fprintf(fptr, "        </UpdateParameters>\n");
	fprintf(fptr, "    </asp:ObjectDataSource>\n");
}

void CSharpAspNetCodeGenerator::print_aspx_formview(FILE* fptr){

	fprintf(fptr, "    <asp:FormView ID=\"fv%s\" runat=\"server\" DataSourceID=\"dsSingle%s\"  \n",
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "    OnItemInserted=\"fv%s_ItemInserted\"  OnItemUpdated=\"fv%s_ItemUpdated\"  \n",
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "     >\n");
	fprintf(fptr, "        <EditItemTemplate>        \n");
	fprintf(fptr, "            <table>\n");
	print_aspx_edit_fields(fptr, tableInfo_->param_list);
	fprintf(fptr, "            <tr><td colspan=\"2\" >\n");
	fprintf(fptr, "                    <asp:Button ID=\"btn_Update\" runat=\"server\"  CausesValidation=\"True\" CommandName=\"Update\"\n");
	fprintf(fptr, "                Text=\"Update\"/> \n");
	fprintf(fptr, "                <asp:Button ID=\"btn_Cancel\" runat=\"server\" CausesValidation=\"False\" CommandName=\"Cancel\"\n");
	fprintf(fptr, "                Text=\"Cancel\"/>\n");
	fprintf(fptr, "                </td>\n");
	fprintf(fptr, "            </tr>            \n");
	fprintf(fptr, "            \n");
	fprintf(fptr, "            \n");
	fprintf(fptr, "            </table>       \n");
	fprintf(fptr, "        \n");
	fprintf(fptr, "        </EditItemTemplate>\n");
	fprintf(fptr, "        <InsertItemTemplate>\n");
	fprintf(fptr, "\t\t\t<table>\n");
	print_aspx_insert_fields(fptr, tableInfo_->param_list);
	fprintf(fptr, "\t\t\t<tr><td colspan=\"2\">\n");
	fprintf(fptr, "            <asp:LinkButton ID=\"InsertButton\" runat=\"server\" CausesValidation=\"True\" CommandName=\"Insert\"\n");
	fprintf(fptr, "                Text=\"Insert\" />\n");
	fprintf(fptr, "            <asp:LinkButton ID=\"InsertCancelButton\" runat=\"server\" CausesValidation=\"False\" CommandName=\"Cancel\"\n");
	fprintf(fptr, "                Text=\"Cancel\" />\n");

	fprintf(fptr, "\t\t\t</td></tr></table>\n");
	fprintf(fptr, "        </InsertItemTemplate>\n");

	fprintf(fptr, "        <ItemTemplate>\n");
	fprintf(fptr, "            <table>\n");


	struct var_list* v_ptr=tableInfo_->param_list;
	//v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "\t\t<tr><td>%s: </td> <td> <asp:Label ID=\"lb_%s\" runat=\"server\" Text='<%%# Bind(\"",
		v_ptr->var_name.c_str(),  v_ptr->var_name.c_str() );
		fprintf(fptr, "%s\") %%>'/></td></tr>\n",  v_ptr->var_name.c_str()  );
		v_ptr=v_ptr->prev;
	}
	fprintf(fptr, "            <tr >\n");
	fprintf(fptr, "                <td >\n");
	fprintf(fptr, "                    <asp:Button ID=\"EditButton\" runat=\"server\" CausesValidation=\"False\" CommandName=\"Edit\"\n");
	fprintf(fptr, "                Text=\"Edit\"></asp:Button>\n");
	fprintf(fptr, "                <asp:Button ID=\"NewButton\" runat=\"server\" CausesValidation=\"False\" CommandName=\"New\"\n");
	fprintf(fptr, "                Text=\"New\"></asp:Button>\n");
	fprintf(fptr, "                </td>\n");
	fprintf(fptr, "            </tr>            \n");
	fprintf(fptr, "            \n");
	fprintf(fptr, "            </table>\n");
	fprintf(fptr, "        </ItemTemplate>\n");
	fprintf(fptr, "    </asp:FormView>\n");
	fprintf(fptr, "    <br />\n");

}
void CSharpAspNetCodeGenerator::print_aspx_gridview(FILE* fptr){
	fprintf(fptr, "    <asp:GridView SkinID=\"ListTable\" ID=\"gv%s\" runat=\"server\" \n\tDataSourceID=\"ds%s\" AutoGenerateColumns=\"False\" \n\tDataKeyNames=\"%s\" AllowPaging=\"True\"  \n\tOnSelectedIndexChanged=\"gv%s_SelectedIndexChanged\">\n",
			tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str(), tableInfo_->param_list->var_name.c_str(),
			tableInfo_->tableName_.c_str());
	fprintf(fptr, "        <Columns>\n");
	/* Make this a function below */
	print_bound_fields(fptr,tableInfo_->param_list);
	fprintf(fptr, "        </Columns>\n");
	fprintf(fptr, "    </asp:GridView>\n");
}

void CSharpAspNetCodeGenerator::print_aspx_header(FILE* fptr){

	fprintf(fptr, "<%%@ Page Language=\"C#\" MasterPageFile=\"~/LeftColMaster.master\" AutoEventWireup=\"true\" CodeFile=\"Manage%s.aspx.cs\" Inherits=\"%s.UI.Manage%s\" Title=\"Manage %s\" %%>\n", tableInfo_->tableName_.c_str(), project_namespace, tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str() );
	fprintf(fptr, "<asp:Content ID=\"Content1\" ContentPlaceHolderID=\"MainContent\" Runat=\"Server\">\n");

}

void CSharpAspNetCodeGenerator::print_aspx_footer(FILE* fptr){
	fprintf(fptr, "</asp:Content>\n");
	fprintf(fptr, "<asp:Content ID=\"Content2\" ContentPlaceHolderID=\"LeftContent\" Runat=\"Server\">\n");
	fprintf(fptr, "</asp:Content>\n");
}

void CSharpAspNetCodeGenerator::print_aspx_validator(FILE* fptr, struct var_list* v_ptr){
	if(v_ptr->options.validator == re_float){
		fprintf(fptr, "<asp:RegularExpressionValidator ID=\"val_%s\" runat=\"server\" ControlToValidate=\"tb_%s\" SetFocusOnError=\"true\" Display=\"Dynamic\"\n"
				"\t\t\tValidationExpression=\"([1-9][0-9]*)|([0-9]*\\.[0-9]*)\" ErrorMessage=\"%s: Please enter a number with or without decimal place \" ToolTip=\"%s: Please Enter a Number with or without decimal place\" >*</asp:RegularExpressionValidator>",
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), 
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str() 
	    );
	} else if (v_ptr->options.validator == re_int){
		fprintf(fptr, "<asp:RegularExpressionValidator ID=\"val_%s\" runat=\"server\" ControlToValidate=\"tb_%s\" SetFocusOnError=\"true\" Display=\"Dynamic\"\n"
				"\t\t\tValidationExpression=\"([1-9][0-9]*)\" ErrorMessage=\"%s: Please enter a number with or without decimal place \" ToolTip=\"%s: Please Enter a Number with or without decimal place\" >*</asp:RegularExpressionValidator>",
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), 
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
	} else if (v_ptr->options.validator == re_alnum ){
		fprintf(fptr, "<asp:RegularExpressionValidator ID=\"val_%s\" runat=\"server\" ControlToValidate=\"tb_%s\" SetFocusOnError=\"true\" Display=\"Dynamic\"\n"
				"\t\t\tValidationExpression=\"([A-Za-z1-9][A-Za-z0-9]*)\" ErrorMessage=\"%s: Please enter a number with or without decimal place \" ToolTip=\"%s: Please Enter a Number with or without decimal place\" >*</asp:RegularExpressionValidator>",
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), 
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str()); 
	} else if (v_ptr->options.validator == re_alnumwsp){
		fprintf(fptr, "<asp:RegularExpressionValidator ID=\"val_%s\" runat=\"server\" ControlToValidate=\"tb_%s\" SetFocusOnError=\"true\" Display=\"Dynamic\"\n"
				"\t\t\tValidationExpression=\"([ \\tA-Za-z1-9][ \\tA-Za-z0-9]*)\" ErrorMessage=\"%s: Please enter a number with or without decimal place \" ToolTip=\"%s: Please Enter a Number with or without decimal place\" >*</asp:RegularExpressionValidator>",
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), 
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str()); 
	} else if (v_ptr->options.validator == req_field ){
		fprintf(fptr, "<asp:RequiredFieldValidator ID=\"val_%s\" runat=\"server\" ControlToValidate=\"tb_%s\" SetFocusOnError=\"true\" Display=\"Dynamic\"\n"
			"ErrorMessage=\"%s: is a required field \" >*</asp:RequiredFieldValidator>",
		v_ptr->var_name.c_str(), v_ptr->var_name.c_str(), v_ptr->var_name.c_str()); 
	}
}

	
void CSharpAspNetCodeGenerator::print_sp_search_key_params(FILE* fptr, bool print_comma){
	cout << "ENTER print_sp_search_key_params: print_comma: " <<  print_comma << endl;
	struct var_list* v_ptr=tableInfo_->param_list;
	if(tableInfo_->has_search_key){
		if(print_comma)
			fprintf(fptr, ",\n");
		while(v_ptr){
			int count=0;
			if(v_ptr->options.search_key){
				fprintf(fptr, "@%s ", v_ptr->var_name.c_str());
				print_sp_types(fptr, v_ptr->var_type);
				if(v_ptr->var_type==NVARCHAR_TYPE
					|| v_ptr->var_type==VARCHAR_TYPE 
					|| v_ptr->var_type==NCHAR_TYPE){
					fprintf(fptr, "(%d)\n", v_ptr->arr_len);
				} 
				++count;
				if(count<tableInfo_->has_search_key){
					fprintf(fptr, ",\n");
				}else fprintf(fptr, "\n");
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		fprintf(fptr, "\n");
	}
}

void CSharpAspNetCodeGenerator::print_sp_search_key_whereclause(FILE* fptr){
	if(tableInfo_->has_search_key){
		struct var_list* v_ptr = tableInfo_->param_list;
		fprintf(fptr, "\t\tWHERE ");
		int count = 0;
		while(v_ptr){
			if(v_ptr->options.search_key){
				fprintf(fptr, "%s.%s ", tableInfo_->tableName_.c_str(), v_ptr->var_name.c_str());
				if(isOfStringType(v_ptr->var_type)){
					fprintf(fptr, "like @%s", v_ptr->var_name.c_str());
				} else {
					fprintf(fptr, "= @%s", v_ptr->var_name.c_str());
				}
				++count;
				if(count<tableInfo_->has_search_key){
					fprintf(fptr, " AND \n");
				}
			}
			v_ptr=v_ptr->prev;
		}
	}
}

void CSharpAspNetCodeGenerator::print_popup_aspx(FILE * fptr)
{

	print_popup_aspx_header(fptr);

	fprintf(fptr, "<body>\n");
	fprintf(fptr, "    <form id=\"form1\" runat=\"server\">\n");
	fprintf(fptr, "    <div>\n");
	fprintf(fptr, "	<asp:ObjectDataSource ID=\"ds%s\" runat=\"server\" SelectMethod=\"Get%ss\"\n",
				tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str() );
	fprintf(fptr, "	    TypeName=\"%s.BLL.Common.Biz%s\" SelectCountMethod=\"Get%ssCount\">    \n",
				project_namespace, tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str() );
	fprintf(fptr, "	</asp:ObjectDataSource>\n");
	fprintf(fptr, "	<asp:GridView ID=\"gv%s\" runat=\"server\" AllowPaging=\"True\" AutoGenerateColumns=\"False\" \n",
				tableInfo_->tableName_.c_str());
	fprintf(fptr, "		 OnRowDataBound=\"gv%s_RowDataBound\"\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "	    DataSourceID=\"ds%s\"  >\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "	    <Columns>\n");
	struct var_list* v_ptr=tableInfo_->param_list;
	while(v_ptr){
		fprintf(fptr, "		    <asp:BoundField DataField=\"%s\" HeaderText=\"%s\"  />\n"
				, v_ptr->var_name.c_str(), v_ptr->var_name.c_str());
		v_ptr=v_ptr->prev;
	}

	fprintf(fptr, "	    </Columns>\n");
	fprintf(fptr, "	</asp:GridView>\n");
	fprintf(fptr, "    </div>\n");
	fprintf(fptr, "    </form>\n");
	fprintf(fptr, "</body>\n");
	fprintf(fptr, "</html>\n");
}


void CSharpAspNetCodeGenerator::print_popup_aspx_header(FILE* fptr){

	fprintf(fptr, "<%%@ Page Language=\"C#\" AutoEventWireup=\"true\" CodeFile=\"%sPopup.aspx.cs\" Inherits=\"Controls_%sPopup\" %%>\n", tableInfo_->tableName_.c_str(), tableInfo_->tableName_.c_str());
	fprintf(fptr, "\n");
	fprintf(fptr, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "<html xmlns=\"http://www.w3.org/1999/xhtml\" >\n");
	fprintf(fptr, "<head runat=\"server\">\n");
	fprintf(fptr, "    <title>Choose %s</title>\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "       <script type=\"text/javascript\" language=\"javascript\">\n");
	fprintf(fptr, "     \n");
	fprintf(fptr, "     \n");
	fprintf(fptr, "    function Set%sName(Name,Code){\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "  \n");
	fprintf(fptr, "        next_query_param_start=window.location.search.indexOf(\"&\");\n");
	fprintf(fptr, "        control1Name=window.location.search.substr(1).substring(9,next_query_param_start-1);\n");
	fprintf(fptr, "        \n");
	fprintf(fptr, "        control2_start=window.location.search.indexOf(\"=\")+1+next_query_param_start;\n");
	fprintf(fptr, "        \n");
	fprintf(fptr, "        control2Name=window.location.search.substr(control2_start);\n");
	fprintf(fptr, "        \n");
	fprintf(fptr, "        window.opener.document.forms[0].elements[control1Name].value=Name;\n");
	fprintf(fptr, "        window.opener.document.forms[0].elements[control2Name].value=Code;\n");
	fprintf(fptr, "     \n");
	fprintf(fptr, "        self.close();\n");
	fprintf(fptr, "    }\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "    </script>\n");
	fprintf(fptr, "</head>\n");
}


void CSharpAspNetCodeGenerator::print_popup_aspx_cs(FILE * fptr){
	fprintf(fptr, "using System;\n");
	fprintf(fptr, "using System.Data;\n");
	fprintf(fptr, "using System.Configuration;\n");
	fprintf(fptr, "using System.Collections;\n");
	fprintf(fptr, "using System.Web;\n");
	fprintf(fptr, "using System.Web.Security;\n");
	fprintf(fptr, "using System.Web.UI;\n");
	fprintf(fptr, "using System.Web.UI.WebControls;\n");
	fprintf(fptr, "using System.Web.UI.WebControls.WebParts;\n");
	fprintf(fptr, "using System.Web.UI.HtmlControls;\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "public partial class Controls_%sPopup : System.Web.UI.Page\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "{\n");
	fprintf(fptr, "	protected void Page_Load(object sender, EventArgs e)\n");
	fprintf(fptr, "	{\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "	}\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "	protected void gv%s_RowDataBound(object sender, GridViewRowEventArgs e)\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "	{\n");
	fprintf(fptr, "		int cell_count = e.Row.Cells.Count;\n");
	fprintf(fptr, "		if (cell_count > 1)\n");
	fprintf(fptr, "		{\n");
	fprintf(fptr, "			int control_count = e.Row.Cells[1].Controls.Count;\n");
	fprintf(fptr, "			HyperLink link = new HyperLink();\n");
	fprintf(fptr, "			link.Text = e.Row.Cells[0].Text;\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "			link.NavigateUrl = \"javascript:Set%sName('\" +\n", tableInfo_->tableName_.c_str());
	fprintf(fptr, "				e.Row.Cells[1].Text\n");
	fprintf(fptr, "				+ \"','\" + e.Row.Cells[0].Text\n");
	fprintf(fptr, "				+ \"');\";\n");
	fprintf(fptr, "			e.Row.Cells[0].Controls.Clear();\n");
	fprintf(fptr, "			e.Row.Cells[0].Controls.Add(link);\n");
	fprintf(fptr, "		}\n");
	fprintf(fptr, "\n");
	fprintf(fptr, "	}\n");
	fprintf(fptr, "}\n");
	fprintf(fptr, "\n");
}


void CSharpAspNetCodeGenerator::print_search_facility_aspx(FILE* fptr)
{
	if(tableInfo_->has_search_key){
		fprintf(fptr, "<table>\n");
		struct var_list* v_ptr=tableInfo_->param_list;
		while(v_ptr){
			//int count=0;
			if(v_ptr->options.search_key){
				fprintf(fptr, "<tr>");
				fprintf(fptr, "<td>%s</td>: ", v_ptr->var_name.c_str());
				fprintf(fptr, "<td><asp:TextBox runat=\"server\" ID=\"tb_search_%s\" /> </td>",
						v_ptr->var_name.c_str() );
				fprintf(fptr, "</tr>\n");
			}
			v_ptr=v_ptr->prev;
		}

		fprintf(fptr, "<tr><td colspan=\"2\"><asp:Button text=\"Search/Refresh\" ID=\"btn_Search_Refresh\" runat=\"server\" onclick=\"btn_Search_Refresh_Click\" /></td></tr>\n");
	
		fprintf(fptr, "</table>\n");
	}
}

void CSharpAspNetCodeGenerator::print_search_facility_aspx_cs(FILE* fptr)
{
	if(tableInfo_->has_search_key){
		fprintf(fptr, "\tprotected void btnReload_Click(object sender, EventArgs e)\n");
		fprintf(fptr, "\t{\n");
		fprintf(fptr, "\t\tgvEmployee.DataBind();\n");
		fprintf(fptr, "\t}\n");
	}
}

void CSharpAspNetCodeGenerator::print_lower_fname(FILE * fptr)
{
	char buffer[MAX_VAR_LEN];
	if(tableInfo_->tableName_.length() < MAX_VAR_LEN-1){
		strcpy(buffer, tableInfo_->tableName_.c_str());
		buffer[0]=tolower(buffer[0]);
		fprintf(fptr, "%s", buffer);
	}
}
