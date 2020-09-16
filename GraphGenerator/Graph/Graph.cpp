#include "Graph.h"
#include "GraphVisitor.h"

using namespace graph;

// Edge 
Edge::Edge(const Edge& edge) {
	to = edge.to; 
	dependencies = edge.dependencies;		// unsigned as values opote kanei deap copy
}

Node* Edge::GetTo() const {
	return to; 
}

Edge::Cardinality Edge::GetCardinality(const DependencyType& depType) const {
	auto it = dependencies.find(depType);
	if (it != dependencies.end()) {
		return it->second; 
	}
	return 0;
}

std::map<Edge::DependencyType, Edge::Cardinality> Edge::GetDependencies() const {
	return dependencies;
}


void Edge::AddDependency(const DependencyType& depType, Edge::Cardinality card) {
	if (dependencies.find(depType) != dependencies.end()) {
		dependencies[depType] += card;
	}
	else {
		dependencies[depType] = card;
	}
}


// Node
Node::Node(const Node& node) {
	data = node.data;
	outEdges = node.outEdges; // shallow copy
	byDestinationID = node.byDestinationID;
}

ID_T Node::GetID() const {
	return data.Get("id").ToString(); 
}

untyped::Object& Node::GetData() {
	return data; 
}

unsigned Node::EdgesSize() const {
	return outEdges.size();
}

void Node::AddEdge(Edge* edge) {
	outEdges.push_back(edge);
	byDestinationID[edge->GetTo()->GetID()] = edge;
}

void Node::AddEdge(Node* to, const Edge::DependencyType& depType, Edge::Cardinality card) {
	if (byDestinationID.find(to->GetID()) != byDestinationID.end()) {
		byDestinationID[to->GetID()]->AddDependency(depType, card);
	}
	else {
		Edge* edge = new Edge(to);
		edge->AddDependency(depType, card);
		outEdges.push_back(edge);
		byDestinationID[to->GetID()] = edge;
	}
}


// Graph 
Node* Graph::GetNode(ID_T id) const {
	Node* node = nullptr;
	if (byID.find(id) != byID.end()) {
		node = byID.find(id)->second; 
	}
	return node;
}

void Graph::AddNode(Node* node) {
	nodes.insert(node);
	byID[node->GetID()] = node;
}


void Graph::AddEdge(Node* from, Node* to, const Edge::DependencyType& depType, Edge::Cardinality card) {
	from->AddEdge(to, depType, card);
}

void Graph::Accept(GraphVisitor* visitor) {
	for (auto it : nodes) {
		visitor->VisitNode(it);
	}
}

void Graph::Accept(GraphVisitor* visitor) const {
	for (auto it : nodes) {
		visitor->VisitNode(it);
	}
}