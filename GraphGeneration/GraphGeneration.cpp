#include "GraphGeneration.h"

using namespace DependenciesMining; 
using namespace DependenciesGraph;

// GraphGenerationSTVisitor
//void GraphGenerationSTVisitor::VisitSymbol(Symbol* s) {}

void GraphGenerationSTVisitor::VisitStructure(Structure* s) {
	Node* node = nullptr;
	if (node = graph.Get(s->GetID()))
		return;
	
	Node* oldCurrNode = currNode;
	currNode = new Node();
	untyped::Object& nodeData = currNode->GetData();

	// Symbol 
	nodeData.Set("id", s->GetID());
	nodeData.Set("name", s->GetName());
	nodeData.Set("namespace", s->GetNamespace());

	untyped::Object srcInfo;
	srcInfo.Set("fileName", s->GetSourceInfo().GetFileName());
	srcInfo.Set("line", (double)s->GetSourceInfo().GetLine());
	srcInfo.Set("column", (double)s->GetSourceInfo().GetColumn());
	nodeData.Set("srcInfo", srcInfo);
	nodeData.Set("classType", s->GetClassTypeAsString());

	// Structure
	nodeData.Set("structureType", s->GetStructureTypeAsString());
	if (s->GetTemplateParent()) {
		VisitStructure(static_cast<Structure*>(s->GetTemplateParent()));
		nodeData.Set("templateParent", s->GetNestedParent()->GetID());
	}
	if (s->GetNestedParent()) {
		VisitStructure(static_cast<Structure*>(s->GetNestedParent()));
		nodeData.Set("nestedParent", s->GetNestedParent()->GetID());
	}
	untyped::Object basesObj;
	for (auto& it : s->GetBases()) {
		auto* base = it.second;
		VisitStructure(static_cast<Structure*>(base));
		node->AddEdge(graph.Get(base->GetID()), Edge::DependencyType::Inherite);
		basesObj.Set("id", base->GetID());
	}
	nodeData.Set("Bases", basesObj);

	untyped::Object friendsObj;
	for (auto& it : s->GetFriends()) {
		auto* friend_ = it.second;
		VisitStructure(static_cast<Structure*>(friend_));
		node->AddEdge(graph.Get(friend_->GetID()), Edge::DependencyType::Inherite);
		friendsObj.Set("id", friend_->GetID());
	}
	nodeData.Set("Friends", friendsObj);

	untyped::Object templArgsObj;
	for (auto& it : s->GetTemplateArguments()) {
		auto* templArg = it.second;
		VisitStructure(static_cast<Structure*>(templArg));
		node->AddEdge(graph.Get(templArg->GetID()), Edge::DependencyType::Inherite);
		templArgsObj.Set("id", templArg->GetID());
	}
	nodeData.Set("templateArguments", templArgsObj);

	// TODO: Definitions and methods - problem with edges and objects - use stack or curr ...
	
	for (auto& it : s->GetFields()) {
		auto* field = it.second;
		VisitDefinition(static_cast<Definition*>(field));
		// ....
	}

	for (auto& it : s->GetMethods()) {
		auto* method = it.second;
		VisitMethod(static_cast<Method*>(method));
		// ....
	}


	
	currNode = oldCurrNode;
}

void GraphGenerationSTVisitor::VisitMethod(Method* s) {
	
}

void GraphGenerationSTVisitor::VisitDefinition(Definition* s) {
	Structure* typeStruct = (Structure*)s->GetType();
	Node* node = nullptr;
	if (!graph.Get(typeStruct->GetID())){
		VisitStructure(typeStruct);
	}
	node = graph.Get(typeStruct->GetID());

	untyped::Object data;
	
	// Symbol 
	data.Set("id", s->GetID());
	data.Set("name", s->GetName());
	data.Set("namespace", s->GetNamespace());

	untyped::Object srcInfo;
	srcInfo.Set("fileName", s->GetSourceInfo().GetFileName());
	srcInfo.Set("line", (double)s->GetSourceInfo().GetLine());
	srcInfo.Set("column", (double)s->GetSourceInfo().GetColumn());
	data.Set("srcInfo", srcInfo);
	data.Set("classType", s->GetClassTypeAsString());

	data.Set("type", s->GetType()->GetID());

}


Graph& GraphGenerationSTVisitor::GetGraph() {
	return graph;
}



/*
void Analyze(const untyped::Value& key, const untyped::Value& value) {

}

void CreateSTGraph(SymbolTable& st) {
	Graph graph; 
	DeepSTVisitor visitor; 
	st.Accept(&visitor);
	const untyped::Object& obj = visitor.GetData();
	obj.ForEach(Analyze); 

}

*/