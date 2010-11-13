#ifndef __SIMPLE_GRAPH_H
#define __SIMPLE_GRAPH_H

#include "tree.h"
#include "stmt.h"

struct GraphNodeType;

struct GraphEdgeType
{
	struct GraphNodeType * endPoint_;
	struct GraphEdgeType * next_;
	GraphEdgeType();
};

struct GraphNodeType
{
	struct TableInfoType * tiPtr_;
	struct GraphEdge * next_;
	GraphNodeType();
};

struct GraphType
{
	vector<GraphNodeType*> graph_;
	GraphType(int nVertices);
};

void ConstructGraph(struct GraphType & p_graph, struct stmt * st_ptr);

#endif /* __SIMPLE_GRAPH_H */

