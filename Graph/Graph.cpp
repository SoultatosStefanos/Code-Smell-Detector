#include "Graph.h"

using namespace DependenciesGraph;

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


void Edge::AddDependency(const Edge::DependencyType& depType, Edge::Cardinality card) {
	if (dependencies.find(depType) != dependencies.end()) {
		dependencies[depType] += card;
	}
	else {
		dependencies[depType] = card;
	}
}


// Node
Node::Node(const Node& node) {
	curr = node.curr;
	outEdges = node.outEdges; // shallow copy
}

Symbol* Node::GetSymbol() const {
	return curr;
}

unsigned Node::EdgesSize() const {
	return outEdges.size();
}

void Node::AddEdge(Edge* edge) {
	outEdges.push_back(edge);
}

bool Node::isAnalyzed() const {
	return outEdges.size() != 0; 
}


std::map<ID_T, Edge*>& Graph::AddEdge(std::map<ID_T, Edge*>& to, const Edge& edge) {
	ID_T id = edge.GetTo()->GetSymbol()->GetID();
	if (to.find(id) == to.end()) {
		to[id] = new Edge(edge);
	}
	else {
		Edge* toEdge = to.find(id)->second;
		for (auto it : edge.GetDependencies()) {
			toEdge->AddDependency(it.first, edge.GetCardinality(it.first));
		}
	}
	return to;
}

std::map<ID_T, Edge*>& Graph::AddEdges(std::map<ID_T, Edge*>& to, const std::map<ID_T, Edge*>& from) {
	for (auto it : from) {
		AddEdge(to, *it.second);
	}
	return to;
}


Node* Graph::GetOrCreateNode(Symbol* symbol) {
	Node* node;
	if (bySymbolID.find(symbol->GetID()) != bySymbolID.end()) {
		node = bySymbolID[symbol->GetID()];
	}
	else {
		node = new Node(symbol);
		nodes.insert(node);
		bySymbolID[symbol->GetID()] = node;
	}
	return node;
}

void freeMap(std::map<ID_T, Edge*> m) {
	for (auto it : m) {
		delete it.second;
	}
}

std::map<ID_T, Edge*> Graph::AnalyzeST(const SymbolTable& st, const Edge::DependencyType& depType) {
	std::map<ID_T, Edge*> m;
	for (auto it : st) {
		Symbol* symbol = it.second;

		if (symbol->GetClassType() == ClassType::Structure) {
			// Destination Structure ID, edge
			std::map<ID_T, Edge*> sm;
			Node* node = GetOrCreateNode(it.second);
			if (node->isAnalyzed()) {
				Edge edge(node);
				edge.AddDependency(depType);
				m = AddEdge(m, edge);
				continue;
			}
			Structure* s = (Structure*)symbol;
			auto m1 = AnalyzeST(s->GetMethods());
			auto m2 = AnalyzeST(s->GetFields(), Edge::DependencyType::ClassField);
			auto m3 = AnalyzeST(s->GetBases(), Edge::DependencyType::Inherite);
			auto m4 = AnalyzeST(s->GetFriends(), Edge::DependencyType::Friend);
			auto m5 = AnalyzeST(s->GetTemplateArguments(), Edge::DependencyType::ClassTemplateArg);
	
			sm = AddEdges(sm, m1);
			sm = AddEdges(sm, m2);
			sm = AddEdges(sm, m3);
			sm = AddEdges(sm, m4);
			sm = AddEdges(sm, m5);

			// free the m1,2,3,4,5 !!!!!!!
			freeMap(m1);
			freeMap(m2);
			freeMap(m3);
			freeMap(m4);
			freeMap(m5);

			// TemplateParent
			if (s->GetTemplateParent()) {
				auto templParent = s->GetTemplateParent();
				Node* toNode = GetOrCreateNode(templParent);
				Edge edge(toNode);
				edge.AddDependency(Edge::DependencyType::ClassTemplateParent);
				sm = AddEdge(sm, edge);
			}

			// nestedParent
			if (s->GetNestedParent()) {
				auto nestedParent = s->GetNestedParent(); 
				Node* toNode = GetOrCreateNode(nestedParent);
				Edge edge(toNode);
				edge.AddDependency(Edge::DependencyType::NestedClass);
				sm = AddEdge(sm, edge);
			}

			for (auto it2 : sm) {
				node->AddEdge(it2.second);
			}

			// vazei kai epistrefei ton eayto toy (an einai base, friend, ..)
			Edge edge(node);
			edge.AddDependency(depType);
			m = AddEdge(m, edge);

		}else if (symbol->GetClassType() == ClassType::Method) {
			Method* meth = (Method*)symbol;
			
			auto m1 = AnalyzeST(meth->GetArguments(), Edge::DependencyType::MethodArg);
			auto m2 = AnalyzeST(meth->GetDefinitions(), Edge::DependencyType::MethodDefinition);
			auto m3 = AnalyzeST(meth->GetTemplateArguments(), Edge::DependencyType::MethodTemplateArg);
			
			// MemberExpr
			std::map<ID_T, Edge*> m4;
			for (auto it2 : meth->GetMemberExpr()) {
				for (auto it3 : it2.second.GetMembers()) {
					auto memberType = it3.GetType();
					Node* toNode = GetOrCreateNode(memberType);
					Edge edge(toNode);
					edge.AddDependency(Edge::DependencyType::MemberExpr);
					m4 = AddEdge(m4, edge);
				}
			}

			m = AddEdges(m, m1);
			m = AddEdges(m, m2);
			m = AddEdges(m, m3);
			m = AddEdges(m, m4);

			// free m1,2,3,4
			freeMap(m1);
			freeMap(m2);
			freeMap(m3);
			freeMap(m4);

			// Return
			if (meth->GetReturnType()) {
				auto returnType = meth->GetReturnType();
				Node* toNode = GetOrCreateNode(returnType);
				Edge edge(toNode);
				edge.AddDependency(Edge::DependencyType::MethodReturn);
				m = AddEdge(m, edge);
			}

		}
		else if (symbol->GetClassType() == ClassType::Definition) {
			Definition* def = (Definition*)symbol;
			Structure* type = (Structure*)def->GetType();
			Node* node = GetOrCreateNode(type);

			Edge edge(node);
			assert(depType != Edge::DependencyType::Undefined);
			edge.AddDependency(depType, 1);
			m = AddEdge(m, edge);
		}
		else {
			assert(0);
		}
	}
	return m;
}


// Graph 
Graph::Graph(const SymbolTable& st) {
	AnalyzeST(st);
}