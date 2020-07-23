#pragma once
#include "Graph.h"

namespace graph {
	class GraphVisitor {
	public:
		virtual void VisitNode(Node* n) = 0;
		virtual void VisitEdge(Edge* e) = 0;
	};
}