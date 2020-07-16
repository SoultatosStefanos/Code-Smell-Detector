#include "GraphGeneration.h"

using namespace dependenciesMining; 
using namespace graph;
using namespace graphGeneration;

// GraphGenerationSTVisitor

void GraphGenerationSTVisitor::VisitStructure(Structure* s) {
	if (graph.GetNode(s->GetID())) {
		if (currNode) {
			assert(currDepType != Undefined_dep_t);
			currNode->AddEdge(graph.GetNode(s->GetID()), currDepType);
		}
		return;
	}
	
	Node* oldCurrNode = currNode;
	Edge::DependencyType oldCurrDepType = currDepType;
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

	graph.AddNode(currNode);

	// Structure
	nodeData.Set("structureType", s->GetStructureTypeAsString());

	if (s->GetTemplateParent()) {
		auto* templateParent = s->GetTemplateParent();
		currDepType = ClassTemplateParent_dep_t;
		VisitStructure(static_cast<Structure*>(templateParent));
		nodeData.Set("templateParent", templateParent->GetID());
	}

	if (s->GetNestedParent()) {
		auto* nestedParent = s->GetNestedParent();
		currDepType = NestedClass_dep_t;
		VisitStructure(static_cast<Structure*>(nestedParent));
		nodeData.Set("nestedParent", nestedParent->GetID());
	}

	untyped::Object basesObj;
	currDepType = Inherite_dep_t;
	for (auto& it : s->GetBases()) {
		auto* base = it.second;
		VisitStructure(static_cast<Structure*>(base));
		basesObj.Set("id", base->GetID());
	}
	nodeData.Set("bases", basesObj);

	untyped::Object friendsObj;
	currDepType = Friend_dep_t;
	for (auto& it : s->GetFriends()) {
		auto* friend_ = it.second;
		VisitStructure(static_cast<Structure*>(friend_));
		friendsObj.Set("id", friend_->GetID());
	}
	nodeData.Set("friends", friendsObj);

	untyped::Object templArgsObj;
	currDepType = ClassTemplateArg_dep_t;
	for (auto& it : s->GetTemplateArguments()) {
		auto* templArg = it.second;
		VisitStructure(static_cast<Structure*>(templArg));
		templArgsObj.Set("id", templArg->GetID());
	}
	nodeData.Set("templateArguments", templArgsObj);
		
	untyped::Object fieldsObj;
	currDepType = ClassField_dep_t;
	for (auto& it : s->GetFields()) {
		auto* field = it.second;
		VisitDefinition(static_cast<Definition*>(field));
		fieldsObj.Set(it.first, innerObj);
	}
	nodeData.Set("fields", fieldsObj);

	untyped::Object methodsObj;
	for (auto& it : s->GetMethods()) {
		auto* method = it.second;
		VisitMethod(static_cast<Method*>(method));
		methodsObj.Set(it.first, innerObj);
	}
	nodeData.Set("methods", methodsObj);

	if (oldCurrNode) {
		assert(oldCurrDepType != Undefined_dep_t);
		oldCurrNode->AddEdge(currNode, oldCurrDepType);
	}

	currNode = oldCurrNode;
	currDepType = oldCurrDepType;
}


void GraphGenerationSTVisitor::VisitMethod(Method* s) {
	Edge::DependencyType oldCurrDepType = currDepType;
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

	// Method
	data.Set("methodType", s->GetMethodTypeAsString());

	if (s->GetReturnType()) {
		auto* returnType = s->GetReturnType();
		currDepType = MethodReturn_dep_t;
		VisitStructure(static_cast<Structure*>(returnType));
		data.Set("returnType", returnType->GetID());
	}
	
	untyped::Object argsObj;
	currDepType = MethodArg_dep_t;
	for (auto& it : s->GetArguments()) {
		auto* args = it.second;
		VisitDefinition(static_cast<Definition*>(args));
		argsObj.Set(it.first, innerObj);
	}
	data.Set("arguments", argsObj);

	untyped::Object defsObj;
	currDepType = MethodDefinition_dep_t;
	for (auto& it : s->GetDefinitions()) {
		auto* def = it.second;
		VisitDefinition(static_cast<Definition*>(def));
		defsObj.Set(it.first, innerObj);
	}
	data.Set("definitions", defsObj);

	untyped::Object templArgsObj;
	currDepType = MethodTemplateArg_dep_t;
	for (auto& it : s->GetTemplateArguments()) {
		auto* templArg = it.second;
		VisitStructure(static_cast<Structure*>(templArg));
		templArgsObj.Set("id", templArg->GetID());
	}
	data.Set("templateArguments", templArgsObj);

	// memberexpr
	untyped::Object memberExprsObj; 
	currDepType = MemberExpr_dep_t;
	for (auto it : s->GetMemberExpr()) {
		auto expr = it.second;
		untyped::Object memberExprObj;

		memberExprObj.Set("expr", expr.GetExpr());
		untyped::Object srcInfo;
		srcInfo.Set("fileName", expr.GetSourceInfo().GetFileName());
		srcInfo.Set("line", (double)expr.GetSourceInfo().GetLine());
		srcInfo.Set("column", (double)expr.GetSourceInfo().GetColumn());
		memberExprObj.Set("srcInfo", srcInfo);

		untyped::Object locEnd;
		locEnd.Set("fileName", expr.GetLocEnd().GetFileName());
		locEnd.Set("line", (double)expr.GetLocEnd().GetLine());
		locEnd.Set("column", (double)expr.GetLocEnd().GetColumn());
		memberExprObj.Set("locEnd", locEnd);

		for (auto it2 : expr.GetMembers()) {
			auto member = it2;
			auto* memberType = it2.GetType();
			untyped::Object memberObj;

			memberObj.Set("name", member.GetName());
			assert(it2.GetType());
			memberObj.Set("type", memberType->GetID());

			untyped::Object locEnd;
			locEnd.Set("fileName", member.GetLocEnd().GetFileName());
			locEnd.Set("line", (double)member.GetLocEnd().GetLine());
			locEnd.Set("column", (double)member.GetLocEnd().GetColumn());
			memberObj.Set("locEnd", locEnd);

			VisitStructure(static_cast<Structure*>(memberType));
			memberExprObj.Set(member.GetName(), memberObj);				// not sure about the key ??
		}

		memberExprsObj.Set(it.first, memberExprObj);
	}
	data.Set("memberExprs", memberExprsObj);

	innerObj.Clear();
	innerObj = data;
	data.Clear();
	currDepType = oldCurrDepType;
}

void GraphGenerationSTVisitor::VisitDefinition(Definition* s) {
	Edge::DependencyType oldCurrDepType = currDepType;
	
	Structure* typeStruct = (Structure*)s->GetType();
	VisitStructure(typeStruct);
	Node* node = graph.GetNode(typeStruct->GetID());


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

	// Definition
	data.Set("type", typeStruct->GetID());

	innerObj.Clear();
	innerObj = data;
	data.Clear();
	currNode->AddEdge(node, oldCurrDepType, 1);
	currDepType = oldCurrDepType;
}


Graph& GraphGenerationSTVisitor::GetGraph() {
	return graph;
}


Graph graphGeneration::GenetareDependenciesGraph(const SymbolTable& st) {
	GraphGenerationSTVisitor visitor;
	st.Accept(&visitor);
	return visitor.GetGraph();
}
