#ifndef __SIMPLE_GRAPH_H
#define __SIMPLE_GRAPH_H

#include "tree.h"
#include "stmt.h"

struct GraphNodeType;

struct GraphEdgeType
{
	int  endNode_;
	struct GraphEdgeType * next_;
	//GraphEdgeType();
	GraphEdgeType(int p_endNode);
};

struct GraphNodeType
{
	struct TableInfoType * tiPtr_;
	struct GraphEdgeType * edge_;
	GraphNodeType(struct TableInfoType * & p_ti_ptr);
};

struct GraphType
{
	vector<GraphNodeType*> graphNodes_;
	GraphType(int nVertices);
};

void ConstructGraph(struct GraphType & p_graph, struct stmt * st_ptr);
void TestGraph(struct GraphType & p_graph);

#endif /* __SIMPLE_GRAPH_H */

