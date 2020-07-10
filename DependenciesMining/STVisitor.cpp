#include "STVisitor.h"
#include "SymbolTable.h"

using namespace DependenciesMining;

//untyped::Object& STVisitor::GetData() {
//	return data;
//}

// DeepSTVisitor
/*
void DeepSTVisitor::VisitSymbol(Symbol* s) {
	data.Set("id", s->GetID());
	data.Set("name", s->GetName());
	data.Set("namespace", s->GetNamespace());

	untyped::Object srcInfo;
	srcInfo.Set("fileName", s->GetSourceInfo().GetFileName());
	srcInfo.Set("line", (double)s->GetSourceInfo().GetLine());
	srcInfo.Set("column", (double)s->GetSourceInfo().GetColumn());
	data.Set("srcInfo", srcInfo);
	data.Set("classType", s->GetClassTypeAsString());
}




void DeepSTVisitor::VisitStructure(Structure* s) {
	untyped::Object structObj;
	
	DeepSTVisitor v0;
	v0.VisitSymbol(s);
	structObj = v0.GetData(); 

	// Structure's data
	structObj.Set("structureType", s->GetStructureTypeAsString());
	if (s->GetTemplateParent())
		structObj.Set("templateParent", s->GetNestedParent()->GetID());
	if (s->GetNestedParent())
		structObj.Set("nestedParent", s->GetNestedParent()->GetID());

	DeepSTVisitor v1, v2;
	ShallowSTVisitor v3, v4, v5;
	s->GetMethods().Accept(&v1);
	s->GetFields().Accept(&v2);
	s->GetBases().Accept(&v3);
	s->GetFriends().Accept(&v4);
	s->GetTemplateArguments().Accept(&v5);

	structObj.Set("methods", v1.GetData());
	structObj.Set("fields", v2.GetData());
	structObj.Set("bases", v3.GetData());
	structObj.Set("friends", v4.GetData());
	structObj.Set("templateArguments", v5.GetData());

	// free v1,2,3.. data
	data.Set(s->GetID(), structObj);
}


void DeepSTVisitor::VisitMethod(Method* s) {
	untyped::Object methodObj;

	DeepSTVisitor v0;
	v0.VisitSymbol(s);
	methodObj = v0.GetData();

	// Method's data
	methodObj.Set("methodType", s->GetMethodTypeAsString());
	if (s->GetReturnType()) {
		methodObj.Set("returnType", s->GetReturnType()->GetID());
	}

	untyped::Object memberExprsObj;
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
			untyped::Object memberObj;

			memberObj.Set("name", member.GetName());	
			assert(it2.GetType());
			memberObj.Set("type", it2.GetType()->GetID());

			untyped::Object locEnd;
			locEnd.Set("fileName", member.GetLocEnd().GetFileName());
			locEnd.Set("line", (double)member.GetLocEnd().GetLine());
			locEnd.Set("column", (double)member.GetLocEnd().GetColumn());
			memberObj.Set("locEnd", locEnd);

			memberExprObj.Set(member.GetName(), memberObj);				// not sure about the key ??
		}

		memberExprsObj.Set(it.first, memberExprObj);
	}
	methodObj.Set("memberExprs", memberExprsObj);

	DeepSTVisitor v1, v2;
	ShallowSTVisitor v3;
	s->GetArguments().Accept(&v1);
	s->GetDefinitions().Accept(&v2);
	s->GetTemplateArguments().Accept(&v3);

	methodObj.Set("arguments", v1.GetData());
	methodObj.Set("definitions", v2.GetData());
	methodObj.Set("templateArguments", v3.GetData());
	
	// free v1,2,3 data

	data.Set(s->GetID(), methodObj);
}


void DeepSTVisitor::VisitDefinition(Definition* s) {
	untyped::Object definitionObj;
	
	DeepSTVisitor v0;
	v0.VisitSymbol(s);
	definitionObj = v0.GetData();

	definitionObj.Set("type", s->GetType()->GetID());

	data.Set(s->GetID(), definitionObj);
}


// ShallowSTVisitor
void ShallowSTVisitor::VisitSymbol(Symbol* s) {}

void ShallowSTVisitor::VisitStructure(Structure* s) {
	data.Set("id", s->GetID());
}

void ShallowSTVisitor::VisitMethod(Method* s) {}

void ShallowSTVisitor::VisitDefinition(Definition* s) {}
*/