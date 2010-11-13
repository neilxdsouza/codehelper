#include "simple_graph.h"
#include "error.h"

GraphEdgeType::GraphEdgeType()
	: endPoint_(0), next_(0)
{ }

GraphNodeType::GraphNodeType()
	: tiPtr_(0), next_(0)
{ }

GraphType::GraphType(int nVertices)
	: graph_(nVertices)
{ }

void ConstructGraph(struct GraphType & p_graph, struct stmt * stmt_ptr)
{
	log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " Reached ");	
}
