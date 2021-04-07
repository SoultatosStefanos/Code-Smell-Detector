#include "GraphGeneration.h"

using namespace dependenciesMining; 
using namespace graph;
using namespace graphGeneration;

// GraphGenerationSTVisitor

void GraphGenerationSTVisitor::VisitStructure(Structure* s) {
	
	if (s->IsUndefined())
		return;
	
	if (graph.GetNode(s->GetID())) {
		if (currNode && currNode->GetID() != s->GetID()) {			// Ignore the self dependencies 
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
		if (!templateParent->IsUndefined()) {
			currDepType = ClassTemplateParent_dep_t;
			VisitStructure(static_cast<Structure*>(templateParent));
			nodeData.Set("templateParent", templateParent->GetID());
		}
	}

	if (s->GetNestedParent()) {
		auto* nestedParent = s->GetNestedParent();
		if (!nestedParent->IsUndefined()) {
			currDepType = NestedClass_dep_t;
			VisitStructure(static_cast<Structure*>(nestedParent));
			nodeData.Set("nestedParent", nestedParent->GetID());
		}
	}

	untyped::Object basesObj;
	currDepType = Inherit_dep_t;
	double index = 0;
	for (auto& it : s->GetBases()) {
		auto* base = it.second;
		if (!((Structure*)base)->IsUndefined()) {
			VisitStructure(static_cast<Structure*>(base));
			basesObj.Set(index++, base->GetID());
		}
	}
	nodeData.Set("bases", basesObj);

	untyped::Object friendsObj;
	index = 0;
	currDepType = Friend_dep_t;
	for (auto& it : s->GetFriends()) {
		auto* friend_ = it.second;
		if (!((Structure*)friend_)->IsUndefined()) {
			VisitStructure(static_cast<Structure*>(friend_));
			friendsObj.Set(index++, friend_->GetID());
		}
	}
	nodeData.Set("friends", friendsObj);

	untyped::Object templArgsObj;
	index = 0;
	currDepType = ClassTemplateArg_dep_t;
	for (auto& it : s->GetTemplateArguments()) {
		auto* templArg = it.second;
		if (!((Structure*)templArg)->IsUndefined()) {
			VisitStructure(static_cast<Structure*>(templArg));
			templArgsObj.Set(index++, templArg->GetID());
		}
	}
	nodeData.Set("templateArguments", templArgsObj);
		
	untyped::Object fieldsObj;
	currDepType = ClassField_dep_t;
	for (auto& it : s->GetFields()) {
		auto* field = it.second;
		if(((Definition*)field)->isStructure()){
			if (!((Definition*)field)->GetType()->IsUndefined()) {
				VisitDefinition(static_cast<Definition*>(field));
				fieldsObj.Set(it.first, innerObj);
			}
		}
	}
	nodeData.Set("fields", fieldsObj);

	untyped::Object methodsObj;
	for (auto& it : s->GetMethods()) {
		auto* method = it.second;
		if (((Method*)method)->IsTrivial())						// Ignore the Trivial methods that compiler creates automatically
			continue;
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
		if (!returnType->IsUndefined()) {
			currDepType = MethodReturn_dep_t;
			VisitStructure(static_cast<Structure*>(returnType));
			data.Set("returnType", returnType->GetID());
		}
	}
	
	untyped::Object argsObj;
	currDepType = MethodArg_dep_t;
	for (auto& it : s->GetArguments()) {
		auto* arg = it.second;
		if (((Definition*)arg)->isStructure()) {
			if (!((Definition*)arg)->GetType()->IsUndefined()) {
				VisitDefinition(static_cast<Definition*>(arg));
				argsObj.Set(it.first, innerObj);
			}
		}
	}
	data.Set("arguments", argsObj);

	untyped::Object defsObj;
	currDepType = MethodDefinition_dep_t;
	for (auto& it : s->GetDefinitions()) {
		auto* def = it.second;
		if (((Definition*)def)->isStructure()) {
			if (!((Definition*)def)->GetType()->IsUndefined()) {
				VisitDefinition(static_cast<Definition*>(def));
				defsObj.Set(it.first, innerObj);
			}
		}
	}
	data.Set("definitions", defsObj);

	untyped::Object templArgsObj;
	double index = 0;
	currDepType = MethodTemplateArg_dep_t;
	for (auto& it : s->GetTemplateArguments()) {
		auto* templArg = it.second;
		if (!((Structure*)templArg)->IsUndefined()) {
			VisitStructure(static_cast<Structure*>(templArg));
			templArgsObj.Set(index++, templArg->GetID());
		}
	}
	data.Set("templateArguments", templArgsObj);

	// memberexpr
	untyped::Object memberExprsObj; 
	currDepType = MemberExpr_dep_t;
	for (auto it : s->GetMemberExpr()) {
		auto expr = it.second;
		untyped::Object memberExprObj;

		if (!expr.GetMembers().size())
			continue;

		memberExprObj.Set("expr", expr.GetExpr());
		untyped::Object srcInfo;
		srcInfo.Set("fileName", expr.GetSourceInfo().GetFileName());
		srcInfo.Set("line", (double)expr.GetSourceInfo().GetLine());
		srcInfo.Set("column", (double)expr.GetSourceInfo().GetColumn());
		memberExprObj.Set("srcInfo", srcInfo);

		untyped::Object membersObj;
		double index2 = 0;
		for (auto it2 : expr.GetMembers()) {
			auto member = it2;
			auto* memberType = it2.GetType();
			if (!memberType->IsUndefined()) {
				untyped::Object memberObj;

				memberObj.Set("name", member.GetName());
				assert(it2.GetType());
				memberObj.Set("type", memberType->GetID());
				memberObj.Set("memberType", member.GetMemberType());

				untyped::Object locEnd;
				locEnd.Set("fileName", member.GetLocEnd().GetFileName());
				locEnd.Set("line", (double)member.GetLocEnd().GetLine());
				locEnd.Set("column", (double)member.GetLocEnd().GetColumn());
				memberObj.Set("locEnd", locEnd);

				/*if (member.GetMemberType() == Value_mem_t)
					currDepType = MemberExpr_Value_dep_t;
				else if (member.GetMemberType() == ClassField_mem_t)
					currDepType = MemberExpr_ClassField_dep_t;
				else 
					currDepType = MemberExpr_MethodDefinition_dep_t;*/

				VisitStructure(static_cast<Structure*>(memberType));
				membersObj.Set(index2++, memberObj);
			}
			memberExprObj.Set("members", membersObj);				
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
	if (typeStruct->IsUndefined())
		assert(0);

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
