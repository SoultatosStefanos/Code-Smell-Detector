#include "SymbolTable.h"
#include "STVisitor.h"
#include "Graph.h"

using namespace DependenciesGraph;
using namespace DependenciesMining;

class GraphGenerationSTVisitor : public STVisitor {
	Graph graph;
	Node* currNode = nullptr;
	untyped::Object* currObj = nullptr;
	Edge::DependencyType currDepType = Edge::DependencyType::Undefined;
public:
	//virtual void VisitSymbol(Symbol* s);
	virtual void VisitStructure(Structure* s);
	virtual void VisitMethod(Method* m);
	virtual void VisitDefinition(Definition* s);
	Graph& GetGraph();
};