#include <iostream>
#include <cstdlib>
#include "std_headers.h"
#include "simple_graph.h"
#include "error.h"
#include "tree.h"
#include "TableCollectionSingleton.hpp"
#include "AbstractCodeGenerator.h"

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
	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");	
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
		std::cout << TableCollectionSingleton::Instance().Tables[i]->tableInfo_->tableName_ << std::endl;
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
				log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " adding edge ");	
				struct  AbstractCodeGenerator * acgen_ptr = 
						TableCollectionSingleton::Instance()
							.my_find_table(v_ptr->options.ref_table_name);
				struct TableInfoType * target_ti_ptr = acgen_ptr->tableInfo_;
				int found_index = -1;
				for(int j=0; j<p_graph.graphNodes_.size(); ++j) {
					cout << "currently at " << p_graph.graphNodes_[j]->tiPtr_->tableName_ << " in search of "
						<< target_ti_ptr->tableName_;
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
					log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
					p_graph.graphNodes_[i]->edge_ = e;
				} else {
					struct GraphEdgeType * e_ptr = p_graph.graphNodes_[i]->edge_;
					log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
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

	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}

void TestGraph(struct GraphType & p_graph)
{
	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");
	for(int i=0; i<p_graph.graphNodes_.size(); ++i) {
		log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " reached here ");	
		if (p_graph.graphNodes_[i]->edge_) {
			GraphNodeType * target_node = p_graph.graphNodes_[p_graph.graphNodes_[i]->edge_->endNode_];
			std::cout << target_node->tiPtr_->tableName_ << " must come before " << 
				p_graph.graphNodes_[i]->tiPtr_->tableName_ << std::endl;
		}
	}
	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}
