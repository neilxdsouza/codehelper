#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include "std_headers.h"
#include "simple_graph.h"
#include "error.h"
#include "tree.h"
#include "TableCollectionSingleton.hpp"
#include "AbstractCodeGenerator.h"
#include "global_variables.h"
#include "time.h"

//GraphEdgeType::GraphEdgeType()
//	: endNode_(0), next_(0)
//{ }

GraphEdgeType::GraphEdgeType(int p_endNode)
	: endNode_(p_endNode), next_(0)
{ }

GraphNodeType::GraphNodeType(struct TableInfoType * & p_ti_ptr)
	: tiPtr_(p_ti_ptr), edge_(0)
{ }

GraphType::GraphType(int nVertices)
{ graphNodes_.reserve(nVertices); }

void ConstructGraph(struct GraphType & p_graph, struct stmt * stmt_ptr)
{
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");	
	/*
	struct stmt* st_ptr_it = stmt_ptr;
	while (stmt_ptr) {
		struct table_decl_stmt * tbl_decl_ptr = dynamic_cast<table_decl_stmt*> (stmt_ptr);
		if (tbl_decl_ptr) {
			p_graph.tiPtr = table_decl_stmt
		}
	}
	*/
	for(int i=0; i<TableCollectionSingleton::Instance().Tables.size(); ++i) {
		//std::cout << TableCollectionSingleton::Instance().Tables[i]->tableInfo_->tableName_ << std::endl;
		p_graph.graphNodes_.push_back( new GraphNodeType(TableCollectionSingleton::Instance().Tables[i]->tableInfo_));
	}
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
	for(int i=0; i<p_graph.graphNodes_.size(); ++i) {
		//cerr << "tiPtr_: " << p_graph.graphNodes_[i]->tiPtr_ << endl;
		struct TableInfoType * ti_ptr = p_graph.graphNodes_[i]->tiPtr_;
		struct var_list * v_ptr = ti_ptr->param_list;
		//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
		//cerr << " reached here " << __FILE__ << __LINE__ << endl;
		while (v_ptr) {
			if (v_ptr->options.ref_table_name != "" && v_ptr->options.many == true) {
			} else if (v_ptr->options.ref_table_name != "" && v_ptr->options.many == false) {
				//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " adding edge ");	
				struct  AbstractCodeGenerator * acgen_ptr = 
						TableCollectionSingleton::Instance()
							.my_find_table(v_ptr->options.ref_table_name);
				struct TableInfoType * target_ti_ptr = acgen_ptr->tableInfo_;
				int found_index = -1;
				for(int j=0; j<p_graph.graphNodes_.size(); ++j) {
					//cout << "currently at " << p_graph.graphNodes_[j]->tiPtr_->tableName_ << " in search of "
					//	<< target_ti_ptr->tableName_;
					if (p_graph.graphNodes_[j]->tiPtr_== target_ti_ptr) {
						found_index = j;
						break;
					}
				}
				if (found_index == -1) {
					stringstream err_msg;
					err_msg << "could not find : " << target_ti_ptr->tableName_ << " in graph" 
						<< endl;
					error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
					exit(1);
				}
				struct GraphEdgeType * e = new struct GraphEdgeType(found_index);
				if (p_graph.graphNodes_[i]->edge_ == 0) {
					//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
					p_graph.graphNodes_[i]->edge_ = e;
				} else {
					struct GraphEdgeType * e_ptr = p_graph.graphNodes_[i]->edge_;
					//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
					while (e_ptr->next_) {
						e_ptr = e_ptr->next_;
					}
					e_ptr->next_ = e;
				}
			} else {
				// ignore 
			}
			v_ptr = v_ptr->prev;
		}
	}

	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}

void TestGraph(struct GraphType & p_graph)
{
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");
	for(int i=0; i<p_graph.graphNodes_.size(); ++i) {
		//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
		if (p_graph.graphNodes_[i]->edge_) {
			GraphNodeType * target_node = p_graph.graphNodes_[p_graph.graphNodes_[i]->edge_->endNode_];
			//std::cout << target_node->tiPtr_->tableName_ << " must come before " << 
			//	p_graph.graphNodes_[i]->tiPtr_->tableName_ << std::endl;
		}
	}
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}

void TopologicalSort(struct GraphType & p_graph)
{
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");	
	vector<int> vec_visited_node(p_graph.graphNodes_.size());
	global_variables::topologicalOrder.resize(p_graph.graphNodes_.size());
	for (int i=0; i< vec_visited_node.size(); ++i) {
		vec_visited_node[i] = 0;
	}
	int place = vec_visited_node.size() - 1;
	for (int v =0; v<p_graph.graphNodes_.size(); ++v) {
		if (vec_visited_node[v] == 0) {
			Sort(p_graph, v, place, vec_visited_node);
		}
	}
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}

void Sort(GraphType & p_graph, int v, int & place, vector<int> & p_vec_visited_node)
{
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");	
	p_vec_visited_node[v]++;

	GraphEdgeType *e = p_graph.graphNodes_[v]->edge_;
	while (e) {
		int w = e->endNode_;
		if (p_vec_visited_node[w] == 0) {
			Sort(p_graph, w, place, p_vec_visited_node);
		}
		e = e->next_;
	}
	global_variables::topologicalOrder[place--] = v;
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}

void PrintSqlScriptInTopologicalOrder(GraphType & p_graph, vector<int> & p_VerticesInTopologicalOrder)
{
	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");	
	using std::stringstream;
	const int DATE_BUF_SZ = 1000;
	char date_time_out_str[DATE_BUF_SZ];
	time_t t = time(0);
	struct tm * current_time = localtime(&t);
	if (current_time==0) {
		stringstream err_msg;
		err_msg << "unable to get current time  " ;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}

	if (strftime(date_time_out_str, sizeof(date_time_out_str), "%F-%T", current_time) ==0 ) {
		stringstream err_msg;
		err_msg << "strftime returned 0" ;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}

	stringstream current_time_str;
	current_time_str << date_time_out_str;

	stringstream output_dir;
	output_dir << global_variables::output_code_directory_prefix
			<< "/" << current_time_str.str();
	if (mkdir(output_dir.str().c_str(), S_IRUSR | S_IWUSR | S_IXUSR |
				S_IRGRP | S_IWGRP | S_IXGRP) != 0) {
		stringstream err_msg;
		err_msg << "unable to create dir with timestamp" ;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}
				 

	
	string create_tables_sql_sh_fname( output_dir.str()
				+ string("/unified_create_tables_sql.sh"));
	fstream create_tables_sql_sh(create_tables_sql_sh_fname.c_str(), ios_base::out | ios_base::trunc);
	if (!create_tables_sql_sh) {
		stringstream err_msg;
		err_msg << "unable to open file for writing: " << create_tables_sql_sh_fname;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}

	string load_rdg_data_sql_sh_fname( output_dir.str()
				+ string("/unified_load_rdg_data_sql.sh"));
	fstream load_rdg_data_sql_sh(load_rdg_data_sql_sh_fname.c_str(), ios_base::out | ios_base::trunc);
	if (!load_rdg_data_sql_sh) {
		stringstream err_msg;
		err_msg << "unable to open file for writing: " << load_rdg_data_sql_sh_fname;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}

	string create_func_sql_sh_fname( output_dir.str()
				+ string("/unified_create_func_sql.sh"));
	fstream create_func_sql_sh(create_func_sql_sh_fname.c_str(), ios_base::out | ios_base::trunc);
	if (!create_func_sql_sh) {
		stringstream err_msg;
		err_msg << "unable to open file for writing: " << create_func_sql_sh;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}

	string drop_tables_sql_fname( output_dir.str()
				+ string("/unified_drop_tables.sql"));
	fstream drop_tables_sql(drop_tables_sql_fname.c_str(), ios_base::out | ios_base::trunc);
	if (!drop_tables_sql) {
		stringstream err_msg;
		err_msg << "unable to open file for writing: " << drop_tables_sql_fname;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}

	string drop_functions_sql_sh_fname( output_dir.str()
				+ string("/unified_drop_func_sql.sh"));
	fstream drop_functions_sql_sh(drop_functions_sql_sh_fname.c_str(), ios_base::out | ios_base::trunc);
	if (!drop_functions_sql_sh) {
		stringstream err_msg;
		err_msg << "unable to open file for writing: " << drop_functions_sql_sh_fname;
		error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
		exit(1);
	}

	for(int i=p_VerticesInTopologicalOrder.size()-1, j=0; i>=0 ; --i, ++j) {
		int graph_node_index = p_VerticesInTopologicalOrder[i];
		int graph_node_drop_index = p_VerticesInTopologicalOrder[j];
		//cout << "graph_node_index: " << graph_node_index << endl;
		if (graph_node_index < 0 || graph_node_index > p_VerticesInTopologicalOrder.size()-1) {
			stringstream err_msg;
			err_msg << "error in TopologicalSort implementation: " 
				<< endl;
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
			exit(1);
		}
		if (graph_node_drop_index < 0 || graph_node_drop_index > p_VerticesInTopologicalOrder.size()-1) {
			stringstream err_msg;
			err_msg << "error in TopologicalSort implementation: " 
				<< endl;
			error(__FILE__, __LINE__, __PRETTY_FUNCTION__, err_msg.str());
			exit(1);
		}

		GraphNodeType * gn = p_graph.graphNodes_[graph_node_index];
		//std::cout << gn->tiPtr_->tableName_ << endl;
		create_tables_sql_sh << "cat " << "../sp_" << gn->tiPtr_->tableName_ 
			<< "_create_postgres.sql >> unified_create_tables.sql" << endl;
		load_rdg_data_sql_sh << "cat " << "../sp_" << gn->tiPtr_->tableName_ 
			<< "_random_data_postgres.sql >> unified_load_rdg_data.sql" << endl;
		create_func_sql_sh << "cat " << "../sp_" << gn->tiPtr_->tableName_ 
			<< "_select_postgres.sql >> unified_create_func.sql" << endl;

		GraphNodeType * gn_drop = p_graph.graphNodes_[graph_node_drop_index];
		//std::cout << gn->tiPtr_->tableName_ << endl;
		drop_tables_sql << "drop table " << gn_drop->tiPtr_->tableName_ << ";\n";

		drop_functions_sql_sh << "cat " << "../sp_" << gn->tiPtr_->tableName_ 
			<< "_drop_insert_postgres.sql >> unified_drop_func.sql" << endl;
		drop_functions_sql_sh << "cat " << "../sp_select_" << gn->tiPtr_->tableName_ 
			<< "_drop_postgres.sql >> unified_drop_func.sql" << endl;



	}

	cout << "unified sql scripts, script generators have been written to : " 
		<< output_dir.str() << " directory." << endl
		<< "Things are done this way so that all scripts are together and match as a set"
		<< endl;


	//log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}
