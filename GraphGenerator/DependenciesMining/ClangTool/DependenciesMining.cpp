#include "DependenciesMining.h"
#include "Utilities.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PreprocessorOptions.h"
//#include "clang/Tooling/CompilationDatabase.h"
#include <vector>

#define CLASS_DECL "ClassDecl"
#define STRUCT_DECL "StructDecl"
#define FIELD_DECL "FieldDecl"
#define METHOD_DECL "MethodDecl"
#define METHOD_VAR_OR_ARG "MethodVarOrArg"

using namespace dependenciesMining;

// ----------------------------------------------------------------------------------------------

DeclarationMatcher ClassDeclMatcher = anyOf(cxxRecordDecl(isClass()).bind(CLASS_DECL), cxxRecordDecl(isStruct()).bind(STRUCT_DECL));
DeclarationMatcher FieldDeclMatcher = fieldDecl().bind(FIELD_DECL);
DeclarationMatcher MethodDeclMatcher = cxxMethodDecl().bind(METHOD_DECL);
DeclarationMatcher MethodVarMatcher = varDecl().bind(METHOD_VAR_OR_ARG);

// ----------------------------------------------------------------------------------------------

SymbolTable dependenciesMining::structuresTable;
std::unordered_map<std::string, Ignored*> dependenciesMining::ignored;

void initializeIgnored(const std::string& ignoredFiles, const std::string& ignoredNamespaces = "") {
	ignored["filePaths"] = new IgnoredFilePaths(ignoredFiles);
	ignored["namespaces"] = new IgnoredNamespaces(ignoredNamespaces);
}

// ----------------------------------------------------------------------------------------------

// Handle all the Classes and Structs and the Bases
void ClassDeclsCallback::run(const MatchFinder::MatchResult& result) {
	const CXXRecordDecl* d;
	Structure structure;
	if ((d = result.Nodes.getNodeAs<CXXRecordDecl>(CLASS_DECL))) {
		structure.SetStructureType(StructureType::Class);
	}
	else if ((d = result.Nodes.getNodeAs<CXXRecordDecl>(STRUCT_DECL))) {
		structure.SetStructureType(StructureType::Struct);
	}
	else {
		assert(0);
	}

	if (isIgnoredDecl(d)) {
		return;
	}

	// gia ta declarations
	if (!(d->isCompleteDefinition())) {
		if (!d->hasDefinition()){									// for templateDefinition Declarations only
			if(!d->getDescribedClassTemplate())
				return;
		}
		else {
			d = d->getDefinition();
		}
	}
	

	// Templates
	if (d->getDescribedClassTemplate()) {	
		structure.SetStructureType(StructureType::TemplateDefinition);
	} 
	else if (d->getKind() == d->ClassTemplatePartialSpecialization) {
		structure.SetStructureType(StructureType::TemplatePartialSpecialization);
	} 
	else if (d->getKind() == d->ClassTemplateSpecialization) {
		if(d->getTemplateSpecializationKind() == 1)
			structure.SetStructureType(StructureType::TemplateInstantiationSpecialization);
		else 
			structure.SetStructureType(StructureType::TemplateFullSpecialization);
	}else if (d->getKind() == d->TemplateTemplateParm) {
		assert(0);
	}
	else if (d->getKind() == d->TemplateTypeParm) {
		assert(0);
	}

	auto srcLocation = result.SourceManager->getPresumedLoc(d->getLocation());
	structure.SetSourceInfo(srcLocation.getFilename(), srcLocation.getLine(), srcLocation.getColumn());
	if (ignored["filePaths"]->isIgnored(srcLocation.getFilename())) {
		return;
	}

	auto structID = GetIDfromDecl(d);	
	//assert(structID); 
	structure.SetName(GetFullStructureName(d));
	structure.SetID(structID);

	// Namespace
	std::string fullEnclosingNamespace = GetFullNamespaceName(d);
	if (ignored["namespaces"]->isIgnored(fullEnclosingNamespace)) {
		return;
	}
	structure.SetNamespace(fullEnclosingNamespace);


	// Templates 
	if (!d->hasDefinition()) {															// Templates that has Declaration only
		assert(structure.GetStructureType() == StructureType::TemplateDefinition);
		structure.SetStructureType(StructureType::Undefined);
		structuresTable.Install(structure.GetID(), structure);
		return;
	}

	if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
	//if (structure.IsTemplateInstantiationSpecialization()){
		// Template parent
		std::string parentName;
		ID_T parentID; 

		Structure* templateParent;
		if (structure.IsTemplateInstantiationSpecialization()) {								// template Instantiation Specialization
			auto* parent = d->getTemplateInstantiationPattern();
			parentID = GetIDfromDecl(parent); 
			//assert(parentID); 
			parentName = GetFullStructureName(parent);
			templateParent = (Structure*)structuresTable.Lookup(parentID);
		}
		else {																					// template Full and Parsial Specialization
			parentName = d->getQualifiedNameAsString();	
			templateParent = (Structure*)structuresTable.Lookup(parentName);
			assert(templateParent);
			parentID = templateParent->GetID();
		}
		if (!templateParent)
			templateParent = (Structure*)structuresTable.Install(parentID, parentName);
		structure.SetTemplateParent(templateParent);

		//Template Arguments		
		auto* temp = (ClassTemplateSpecializationDecl*)d;
		for (unsigned i = 0; i < temp->getTemplateArgs().size(); ++i) {
			auto templateArg = temp->getTemplateArgs()[i];
			TemplateArgsVisit(templateArg, [](TemplateArgument templateArg, Structure *structure) {
					RecordDecl* d = nullptr;
					if (templateArg.getKind() == TemplateArgument::Template) {
						d = (RecordDecl*)templateArg.getAsTemplateOrTemplatePattern().getAsTemplateDecl()->getTemplatedDecl();
						if (!d)													// a set of function templates 
							return;
					}

					if (d || GetTemplateArgType(templateArg)->isStructureOrClassType()) {
						if (!d)
							d = GetTemplateArgType(templateArg)->getAsCXXRecordDecl();			
						std::string argStructName = GetFullStructureName(d);
						auto argStructID = GetIDfromDecl(d);	
						//assert(argStructID);
						Structure* argStruct = (Structure*)structuresTable.Lookup(argStructID);
						if(argStruct == nullptr)
							argStruct = (Structure*)structuresTable.Install(argStructID, argStructName);
						structure->InstallTemplateSpecializationArguments(argStructID, argStruct);
					}
				}, &structure);			
		}
	}
	

	// Bases
	for(auto it : d->bases()){
		auto* baseRecord = it.getType()->getAsCXXRecordDecl();	
		if (baseRecord == nullptr)										// otan base einai template or partial specialization Ignored
			continue;
		std::string baseName = GetFullStructureName(baseRecord);
		auto baseID = GetIDfromDecl(baseRecord); 
		//assert(baseID);
		Structure* base = (Structure*)structuresTable.Lookup(baseID);
		if (!base)
			base = (Structure*)structuresTable.Install(baseID, baseName);
		structure.InstallBase(baseID, base);
	}

	// Friends 
	for (auto* it : d->friends()) {
		auto* type = it->getFriendType();
		if (type) {																							// Classes
			auto parent = type->getType()->getAsCXXRecordDecl();
			if (!parent)																					// ignore decls that does not have definitions
				continue;
			auto parentID = GetIDfromDecl(parent); 
			//assert(parentID);
			std::string parentName = GetFullStructureName(type->getType()->getAsCXXRecordDecl()) ;
			Structure* parentStructure = (Structure*)structuresTable.Lookup(parentID);
			if (!parentStructure)
				parentStructure = (Structure*)structuresTable.Install(parentID, parentName);
			structure.InstallFriend(parentID, parentStructure);
		}
		else {																								// Methods			
			auto* decl = it->getFriendDecl();
			auto kind = decl->getKind();
			if (decl->getKind() == d->CXXMethod || decl->getKind() == d->FunctionTemplate) {
				CXXMethodDecl* methodDecl; 
				if (decl->getKind() == d->FunctionTemplate) {												// Template Methods				 		
					auto funcdecl = ((FunctionTemplateDecl*)decl)->getTemplatedDecl();
					auto parent = funcdecl->getParent();
					if(!parent->isRecord())																	// Ignore Template Function
						continue;
					methodDecl = (CXXMethodDecl*)funcdecl;												
				}
				else {
					methodDecl = (CXXMethodDecl*)decl;
				}
				std::string methodName = GetFullMethodName(methodDecl);
				auto* parentClass = methodDecl->getParent();
				auto parentClassID = GetIDfromDecl(parentClass);	
				//assert(parentClassID);
				Structure* parentStructure = (Structure*)structuresTable.Lookup(parentClassID);
				if (!parentStructure) continue;
				// meta thn allagh se ids ws keys den krataw info gia to idio to method alla mono gia to structure pou anoikei
				structure.InstallFriend(parentClassID, parentStructure);
			}
			else if (decl->isTemplateDecl()) {																// Template Classes
				auto recdecl = (RecordDecl*)((TemplateDecl*)decl)->getTemplatedDecl();			
				auto structureDefinition = recdecl->getDefinition(); 
				if (!structureDefinition)																	// ignore decls that does not have definitions
					continue;	

				auto parentID = GetIDfromDecl(structureDefinition);
				auto parentName = GetFullStructureName(structureDefinition);
				Structure* parentStructure = (Structure*)structuresTable.Lookup(parentID);
				if (!parentStructure) {
					parentStructure = (Structure*)structuresTable.Lookup(parentName);
					if (!parentStructure) {
						parentStructure = (Structure*)structuresTable.Install(parentID, parentName);
					}
				}
				structure.InstallFriend(parentID, parentStructure);
			}
		}
	}

	// Members
	if (d->isCXXClassMember()) {
		const auto* parent = d->getParent();
		assert(parent);
		std::string parentName = GetFullStructureName((RecordDecl*)parent);
		auto parentID = GetIDfromDecl((RecordDecl*)parent);
		//assert(parentID);
		if (isIgnoredDecl((RecordDecl*)parent)) {
			return;
		}
		if (parentName != structure.GetName()) {
			Structure* parentStructure = (Structure*)structuresTable.Lookup(parentID);
			auto* inst = d->getInstantiatedFromMemberClass();
			structure.SetNestedParent(parentStructure);
			parentStructure->InstallNestedClass(structure.GetID(), (Structure*)structuresTable.Install(structure.GetID(), structure.GetName()));
		}
	}
	structuresTable.Install(structure.GetID(), structure);
}

// ----------------------------------------------------------------------------------------------
// Hanlde all the Fields in classes/structs (non structure fields)
void FeildDeclsCallback::installFundamentalField(const MatchFinder::MatchResult& result) {
	if (const FieldDecl* d = result.Nodes.getNodeAs<FieldDecl>(FIELD_DECL)) {
		auto* parent = d->getParent();

		// Ignored
		auto srcLocation = result.SourceManager->getPresumedLoc(d->getLocation());

		std::string parentName = GetFullStructureName(parent);
		std::string typeName = d->getType().getAsString();
		ID_T parentID = GetIDfromDecl(parent);
		//if (d->getType()->isPointerType() || d->getType()->isReferenceType()) {
		//	typeName = GetFullStructureName(d->getType()->getPointeeType()->getAsRecordDecl()); // CXX
		//	typeID = GetIDfromDecl(d->getType()->getPointeeType()->getAsRecordDecl());			// CXX
		//}
		//else {
		//	typeName = GetFullStructureName(d->getType()->getAsCXXRecordDecl());
		//	typeID = GetIDfromDecl(d->getType()->getAsCXXRecordDecl());
		//}




		Structure* parentStructure = (Structure*)structuresTable.Lookup(parentID);

		//Structure* typeStructure = (Structure*)structuresTable.Lookup(typeID);


		//if (parentStructure->IsTemplateInstantiationSpecialization())		// insertion speciallization inherit its dependencies from the parent template
		//	return;
		//if (!typeStructure)
		//	typeStructure = (Structure*)structuresTable.Install(typeID, typeName);

		auto fieldID = GetIDfromDecl(d);
		//assert(fieldID);

		Definition field(fieldID, d->getQualifiedNameAsString(), parentStructure->GetNamespace());
		field.SetSourceInfo(srcLocation.getFilename(), srcLocation.getLine(), srcLocation.getColumn());
		field.SetFundamental(typeName);
		parentStructure->InstallField(fieldID, field);

	}
}



// Hanlde all the Fields in classes/structs (structure fields)
void FeildDeclsCallback::run(const MatchFinder::MatchResult& result) {
	if (const FieldDecl* d = result.Nodes.getNodeAs<FieldDecl>(FIELD_DECL)) {
		auto* parent = d->getParent();

		// Ignored
		auto srcLocation = result.SourceManager->getPresumedLoc(d->getLocation());
		if (ignored["filePaths"]->isIgnored(srcLocation.getFilename())) {
			return;
		}
		
		if (ignored["namespaces"]->isIgnored(GetFullNamespaceName(parent))) {
			return;
		}

		if(isIgnoredDecl(parent)) {
			return;
		}

		if (!isStructureOrStructurePointerType(d->getType())) {
			installFundamentalField(result);
			return;
		}

		if (parent->isClass() || parent->isStruct()) {
			std::string parentName = GetFullStructureName(parent);
			std::string typeName;
			ID_T parentID = GetIDfromDecl(parent);
			ID_T typeID = "";
			if (d->getType()->isPointerType() || d->getType()->isReferenceType()) {
				typeName = GetFullStructureName(d->getType()->getPointeeType()->getAsRecordDecl()); // CXX
				typeID = GetIDfromDecl(d->getType()->getPointeeType()->getAsRecordDecl());			// CXX
			}
			else {
				typeName = GetFullStructureName(d->getType()->getAsCXXRecordDecl());
				typeID = GetIDfromDecl(d->getType()->getAsCXXRecordDecl());
			}
			//assert(parentID);
			//assert(typeID);

			Structure* parentStructure = (Structure*)structuresTable.Lookup(parentID);
			Structure* typeStructure = (Structure*)structuresTable.Lookup(typeID);
			if (parentStructure->IsTemplateInstantiationSpecialization())		// insertion speciallization inherit its dependencies from the parent template
				return;
			if (!typeStructure)
				typeStructure = (Structure*)structuresTable.Install(typeID, typeName);

			auto fieldID = GetIDfromDecl(d);
			//assert(fieldID);
			Definition field(fieldID, d->getQualifiedNameAsString(), parentStructure->GetNamespace(), typeStructure);
			field.SetSourceInfo(srcLocation.getFilename(), srcLocation.getLine(), srcLocation.getColumn());
			parentStructure->InstallField(fieldID, field);
		}
	}
}

// ----------------------------------------------------------------------------------------------

// Handle all the Methods
void MethodDeclsCallback::run(const MatchFinder::MatchResult& result) {
	if (const CXXMethodDecl* d = result.Nodes.getNodeAs<CXXMethodDecl>(METHOD_DECL)) {
		const RecordDecl* parent = d->getParent();
		std::string parentName = GetFullStructureName(parent);
		auto parentID = GetIDfromDecl(parent);
		auto methodID = GetIDfromDecl(d);
		//assert(parentID);
		//assert(methodID);

		if(!(d->isThisDeclarationADefinition())){
			return;
		}

		// Ignored
		auto srcLocation = result.SourceManager->getPresumedLoc(d->getLocation());
		if (ignored["filePaths"]->isIgnored(srcLocation.getFilename())) {
			return;
		}
		if (ignored["namespaces"]->isIgnored(GetFullNamespaceName(parent))) {
			return;
		}

		if (isIgnoredDecl(parent)) {
			return;
		}

		Structure* parentStructure = (Structure*)structuresTable.Lookup(parentID);
		assert(parentStructure);
		/*if (!parentStructure) {
			parentStructure = (Structure*)structuresTable.Install(parentID, parentName);
		}*/
		Method method(methodID, GetFullMethodName(d), parentStructure->GetNamespace());
		method.SetSourceInfo(srcLocation.getFilename(), srcLocation.getLine(), srcLocation.getColumn());

		// Method's Type
		if (d->getDeclKind() == d->CXXConstructor) {
			if (d->isTrivial()) {
				method.SetMethodType(MethodType::Constructor_Trivial);
			}
			else {
				method.SetMethodType(MethodType::Constructor_UserDefined);
			}
		}
		else if (d->getDeclKind() == d->CXXDestructor) {
			if (d->isTrivial()) {
				method.SetMethodType(MethodType::Destructor_Trivial);
			}
			else {
				method.SetMethodType(MethodType::Destructor_UserDefined);
			}
		}
		else if (d->isOverloadedOperator()) {
			if (d->isTrivial()) {
				method.SetMethodType(MethodType::OverloadedOperator_Trivial);
			}
			else {
				method.SetMethodType(MethodType::OverloadedOperator_UserDefined);
			}
		}
		else if (d->getTemplatedKind()) {
			if (d->getTemplatedKind() == d->TK_FunctionTemplate) {
				method.SetMethodType(MethodType::TemplateDefinition);
			}
			else if (d->getTemplatedKind() == d->TK_FunctionTemplateSpecialization || d->getTemplatedKind() == d->TK_DependentFunctionTemplateSpecialization) {
				if (d->isTemplateInstantiation()) {
					method.SetMethodType(MethodType::TemplateInstantiationSpecialization);
				}
				else {
					method.SetMethodType(MethodType::TemplateFullSpecialization);
				}
			}
			else if (d->getTemplatedKind() == d->TK_MemberSpecialization) {
				method.SetMethodType(MethodType::UserMethod);		// user method on teplate class
			}
			else {
				std::cout << d->getTemplatedKind() << "\n\n";
				assert(0);
			}
		}
		else {
			method.SetMethodType(MethodType::UserMethod);
		}

		//Template
		if (method.IsTemplateFullSpecialization() || method.IsTemplateInstantiationSpecialization()) {
		/*	// Tempalte Method's parent
			Method* templateParentMethod = nullptr;
			std::string parentMethodName = GetFullMethodName(d);
			size_t start = parentMethodName.find("<");
			size_t end = parentMethodName.find(">");
			parentMethodName.erase(parentMethodName.begin() + start, parentMethodName.begin() + end + 1);
			if (parentStructure->IsTemplateFullSpecialization() || parentStructure->IsTemplateInstantiationSpecialization()) {
				
				templateParentMethod = parentStructure->GetTemplateParent()->GetMethod(parentMethodName);
			}
			else {
				templateParentMethod = parentStructure->GetMethod(parentMethodName);
			}
			assert(templateParentMethod);
			method.SetTemplateParent(templateParentMethod);*/
			//Template Arguments		
			auto args = d->getTemplateSpecializationArgs()->asArray();
			for (auto it : args) {
				TemplateArgsVisit(it, [](TemplateArgument templateArg, Method* method) {
					RecordDecl* d = nullptr;
					if (templateArg.getKind() == TemplateArgument::Template) {
						d = (RecordDecl*)templateArg.getAsTemplateOrTemplatePattern().getAsTemplateDecl()->getTemplatedDecl();
						if (!d)													// a set of function templates 
							return;
					}
					else if (templateArg.getKind() == TemplateArgument::Integral) {
						return;
					}
					if (d || GetTemplateArgType(templateArg)->isStructureOrClassType()) {
						if (!d)
							d = GetTemplateArgType(templateArg)->getAsCXXRecordDecl();
						std::string argStructName = GetFullStructureName(d);
						auto argStructID = GetIDfromDecl(d);
						//assert(argStructID);
						Structure* argStruct = (Structure*)structuresTable.Lookup(argStructID);
						if (argStruct == nullptr)
							argStruct = (Structure*)structuresTable.Install(argStructID, argStructName);
						method->InstallTemplateSpecializationArguments(argStructID, argStruct);
					}
					}, &method);
			}
		}

		//Return
		auto returnType = d->getReturnType();
		if (isStructureOrStructurePointerType(returnType)) {
			std::string typeName;
			ID_T typeID;
			if (returnType->isPointerType() || returnType->isReferenceType()) {
				typeName = GetFullStructureName(returnType->getPointeeType()->getAsCXXRecordDecl());
				typeID = GetIDfromDecl(returnType->getPointeeType()->getAsCXXRecordDecl());
			}
			else {
				typeName = GetFullStructureName(returnType->getAsCXXRecordDecl());
				typeID = GetIDfromDecl(returnType->getAsCXXRecordDecl());
			}
			//assert(typeID);
			Structure* typeStructure = (Structure*)structuresTable.Lookup(typeID);
			if (!typeStructure)
				typeStructure = (Structure*)structuresTable.Install(typeID, typeName);
			method.SetReturnType(typeStructure);
		}

		currentMethod = (Method*)parentStructure->InstallMethod(methodID, method);
		sm = result.SourceManager;
		
		// Body - MemberExpr
		auto* body = d->getBody();
		FindMemberExprVisitor visitor; 

		literal_count = 0;
		statement_count = 0;
		visitor.TraverseStmt(body);

		currentMethod->SetLiterals(literal_count);
		currentMethod->SetStatements(statement_count);

		currentMethod = nullptr;


	}
}

// ----------------------------------------------------------------------------------------------

bool MethodDeclsCallback::FindMemberExprVisitor::TraverseStmt(Stmt* stmt) {
	if (!stmt)
		return true;

	if(stmt->getStmtClass() == Stmt::StmtClass::CompoundStmtClass)
		MethodDeclsCallback::statement_count += std::distance(stmt->child_begin(), stmt->child_end());


	std::string class_name = stmt->getStmtClassName();
	std::cout << class_name << std::endl;
	if (class_name.find("Literal") != std::string::npos)
		MethodDeclsCallback::literal_count++;


	RecursiveASTVisitor<FindMemberExprVisitor>::TraverseStmt(stmt);
	return true;
}


// Handle all the MemberExpr in a method 
bool MethodDeclsCallback::FindMemberExprVisitor::VisitMemberExpr(MemberExpr* memberExpr) {
	auto* decl = memberExpr->getMemberDecl();
	auto type = memberExpr->getType();
	auto* base = memberExpr->getBase();

	if (base) {
		base = base->IgnoreUnlessSpelledInSource();				// clean all the invisble AST nodes that may surround this stmt

		if (base->getStmtClass() == memberExpr->CXXThisExprClass)	// ignore this
			return true;

		std::string str; 
		Method::Member::MemberType memType;
		if (base->getStmtClass() == memberExpr->DeclRefExprClass) {
			str = "__LOCAL DEF__";
			memType = MethodDefinition_mem_t;
		}
		else if (base->getStmtClass() == memberExpr->MemberExprClass && ((MemberExpr*)base)->getBase()->getStmtClass() == memberExpr->CXXThisExprClass) {
			str = "__CLASS FIELD__";
			memType = ClassField_mem_t; 
		}
		else if (base->getStmtClass() == memberExpr->CXXMemberCallExprClass) {
			str = "__CLASS METHOD__";
			memType = ClassMethod_mem_t;
		}
		else {
			str = "__??__";
			memType = Value_mem_t;
		}
			
		auto baseType = base->getType();
		auto baseRange = base->getSourceRange();
		auto baseScLocationBegin = MethodDeclsCallback::sm->getPresumedLoc(baseRange.getBegin());
		auto baseScLocationEnd = MethodDeclsCallback::sm->getPresumedLoc(baseRange.getEnd());
		SourceInfo baseLocBegin(baseScLocationBegin.getFilename(), baseScLocationBegin.getLine(), baseScLocationBegin.getColumn());
		SourceInfo baseLocEnd(baseScLocationEnd.getFilename(), baseScLocationEnd.getLine(), baseScLocationEnd.getColumn());
		std::string exprString = str; 
		Method::MemberExpr methodMemberExpr("__DUMMY EXPR__", baseLocEnd, baseLocBegin.GetFileName(), baseLocBegin.GetLine(), baseLocBegin.GetColumn());
			
		std::string typeName;
		ID_T typeID;
		if (baseType->isPointerType() || baseType->isReferenceType()) {
			typeName = GetFullStructureName(baseType->getPointeeType()->getAsCXXRecordDecl());
			typeID = GetIDfromDecl(baseType->getPointeeType()->getAsCXXRecordDecl());
		}
		else {
			typeName = GetFullStructureName(baseType->getAsCXXRecordDecl());
			typeID = GetIDfromDecl(baseType->getAsCXXRecordDecl());
		}
		Structure* typeStructure = (Structure*)structuresTable.Lookup(typeID);
		if (!typeStructure)
			typeStructure = (Structure*)structuresTable.Install(typeID, typeName);

		Method::Member member(str, typeStructure, baseLocEnd, memType);
		MethodDeclsCallback::currentMethod->InsertMemberExpr(methodMemberExpr, member, baseLocBegin.toString());
		
	}
		
	auto range = memberExpr->getSourceRange();
	auto srcLocationBegin = MethodDeclsCallback::sm->getPresumedLoc(range.getBegin());
	auto srcLocationEnd = MethodDeclsCallback::sm->getPresumedLoc(range.getEnd());
	SourceInfo locBegin(srcLocationBegin.getFilename(), srcLocationBegin.getLine(), srcLocationBegin.getColumn());
	SourceInfo locEnd(srcLocationEnd.getFilename(), srcLocationEnd.getLine(), srcLocationEnd.getColumn());
	std::string exprString;

	//if (decl->getKind() == decl->CXXMethod) {
	//	auto end = sm->getCharacterData(range.getEnd());
	//	int openCount = 0, closeCount = 0;
	//	while (!(openCount == closeCount && openCount)) {
	//		if (*end == '(') {
	//			openCount++;
	//		}
	//		else if (*end == ')') {
	//			closeCount++;
	//		}
	//		end++;
	//	}
	//	const char* beginstr = sm->getCharacterData(range.getBegin());
	//	exprString = std::string(beginstr, end);
	//}
	//else {
	//	exprString = std::string(sm->getCharacterData(range.getBegin()), sm->getCharacterData(range.getEnd())) + decl->getNameAsString();
	//}

	Method::MemberExpr methodMemberExpr(exprString, locEnd, locBegin.GetFileName(), locBegin.GetLine(), locBegin.GetColumn());
	MethodDeclsCallback::currentMethod->UpdateMemberExpr(methodMemberExpr, locBegin.toString());

	/*if (!isStructureOrStructurePointerType(type)) {
		if (decl->getKind() == decl->CXXMethod) {
			CXXMethodDecl* methodDecl = (CXXMethodDecl*)decl;
			type = methodDecl->getReturnType();
			//if (!isStructureOrStructurePointerType(type)) {
				MethodDeclsCallback::currentMethod->UpdateMemberExpr(methodMemberExpr, locBegin.toString());	// to get the full expr if I have fields with not a class type
				return true;
			//}
		}
		else {
			MethodDeclsCallback::currentMethod->UpdateMemberExpr(methodMemberExpr, locBegin.toString());
			return true;
		}
	}

	std::string typeName;
	ID_T typeID; 
	if (type->isPointerType() || type->isReferenceType()) {
		typeName = GetFullStructureName(type->getPointeeType()->getAsCXXRecordDecl());
		typeID = GetIDfromDecl(type->getPointeeType()->getAsCXXRecordDecl());
	}
	else {
		typeName = GetFullStructureName(type->getAsCXXRecordDecl());
		typeID = GetIDfromDecl(type->getAsCXXRecordDecl());
	}
	//assert(typeID); 
	Structure* typeStructure = (Structure*)structuresTable.Lookup(typeID);
	if (!typeStructure)
		typeStructure = (Structure*)structuresTable.Install(typeID, typeName);

	Method::Member member(decl->getNameAsString(), typeStructure, locEnd, Value_mem_t);

	MethodDeclsCallback::currentMethod->InsertMemberExpr(methodMemberExpr, member, locBegin.toString());*/

	return true;
}

// ----------------------------------------------------------------------------------------------

// Handle Method's Vars and Args
void MethodVarsCallback::run(const MatchFinder::MatchResult& result) {
	if (const VarDecl* d = result.Nodes.getNodeAs<VarDecl>(METHOD_VAR_OR_ARG)) {
		auto* parentMethodDecl = d->getParentFunctionOrMethod();

		// Ignore the methods declarations 
		if (!parentMethodDecl || !(((CXXMethodDecl*)parentMethodDecl)->isThisDeclarationADefinition())) {
			return;
		}

		if (d->isLocalVarDeclOrParm() && parentMethodDecl->getDeclKind() == d->CXXMethod) {	// including params
		//if(d->isFunctionOrMethodVarDecl() && parentMethod->getDeclKind() == d->CXXMethod){		// excluding params	- d->isFunctionOrMethodVarDecl()-> like isLocalVarDecl() but excludes variables declared in blocks?.		
			auto* parentClass = (CXXRecordDecl*)parentMethodDecl->getParent();

			if (isIgnoredDecl(parentClass)) {
				return;
			}

			auto srcLocation = result.SourceManager->getPresumedLoc(d->getLocation());
			if (ignored["filePaths"]->isIgnored(srcLocation.getFilename())) {
				return;
			}
			if (ignored["namespaces"]->isIgnored(GetFullNamespaceName(parentClass))) {
				return;
			}
			
			auto parentClassName = GetFullStructureName(parentClass);
			auto parentMethodName = GetFullMethodName((CXXMethodDecl*)parentMethodDecl);
			auto parentClassID = GetIDfromDecl(parentClass);
			auto parentMethodID = GetIDfromDecl((CXXMethodDecl*)parentMethodDecl);
			//assert(parentClassID);
			//assert(parentMethodID);
			Structure* parentStructure = (Structure*)structuresTable.Lookup(parentClassID);
			Method* parentMethod = (Method*)parentStructure->LookupMethod(parentMethodID);
			//assert(parentMethod);
			
			// remove from TemplateInstantiationSpecialization methods the decletarions and arguments 
			//if (parentMethod->isTemplateInstantiationSpecialization()) {
			//	return;
			//}

			std::string typeName;
			ID_T typeID;
			auto defID = GetIDfromDecl(d);
			Definition* def = nullptr;

			if (isStructureOrStructurePointerType(d->getType())) {
				if (d->getType()->isPointerType() || d->getType()->isReferenceType()) {
					typeName = GetFullStructureName(d->getType()->getPointeeType()->getAsCXXRecordDecl());
					typeID = GetIDfromDecl(d->getType()->getPointeeType()->getAsCXXRecordDecl());
				}
				else {
					typeName = GetFullStructureName(d->getType()->getAsCXXRecordDecl());
					typeID = GetIDfromDecl(d->getType()->getAsCXXRecordDecl());
				}
				//assert(typeID);
				Structure* typeStructure = (Structure*)structuresTable.Lookup(typeID);
				if (!typeStructure)
					typeStructure = (Structure*)structuresTable.Install(typeID, typeName);
				//assert(defID);
				def = new Definition (defID, d->getQualifiedNameAsString(), parentMethod->GetNamespace(), typeStructure);
				def->SetSourceInfo(srcLocation.getFilename(), srcLocation.getLine(), srcLocation.getColumn());
			}
			else {

				typeName = d->getType().getAsString();
				def = new Definition (defID, d->getQualifiedNameAsString(), parentStructure->GetNamespace());
				def->SetSourceInfo(srcLocation.getFilename(), srcLocation.getLine(), srcLocation.getColumn());
				def->SetFundamental(typeName);
			}


			
			
			if (d->isLocalVarDecl()) {
				parentMethod->InstallDefinition(defID, *def);
			}
			else {
				parentMethod->InstallArg(defID, *def);
			}

			delete def;
		}
	}
}


// ----------------------------------------------------------------------------------------------

/*
	returns nullptr on fail.
*/
std::unique_ptr<CompilationDatabase> dependenciesMining::LoadCompilationDatabase(const char* cmpDBPath) {
	std::string errorMsg;
	auto cmpDB = CompilationDatabase::autoDetectFromSource(cmpDBPath, errorMsg);
	if (!cmpDB) { // Input error, exit program.
		std::cerr << "In '" << cmpDBPath << "'\n";
		std::cerr << errorMsg << "\n";
		std::cerr << "Make sure Compilation Database .json is named: 'compile_commands.json'\n";
		return nullptr;
	}

	/*auto srcs = cmpDB->getAllFiles();
	std::cout << "Files from Compilation Database:\n\n";
	for (auto file : srcs) {
		std::cout << file << std::endl;
	}*/
	return cmpDB;
}

/*
	Clang Tool Creation
*/
static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nA help message for this specific tool can be added afterwards..\n");

int dependenciesMining::CreateClangTool(const char* cmpDBPath, std::vector<std::string> srcs, const char* ignoredFilePaths, const char* ignoredNamespaces) {
	ClangTool* Tool;
	std::unique_ptr<CompilationDatabase> cmpDB;
	CommonOptionsParser *OptionsParser = nullptr;

	if (cmpDBPath == nullptr) {
		int argc = 3;
		const char* argv[3];
		argv[0] = "";
		argv[1] = "";
		argv[2] = "--";
		OptionsParser = new CommonOptionsParser(argc, argv, MyToolCategory);
		Tool = new ClangTool(OptionsParser->getCompilations(), srcs);
	}
	else {
		cmpDB = LoadCompilationDatabase(cmpDBPath);
		if (!cmpDB)
			return -1;
		Tool = new ClangTool(*cmpDB, cmpDB->getAllFiles());
	}


	
	clang::CompilerInstance comp;
	comp.getPreprocessorOpts().addMacroDef("_W32BIT_");

	initializeIgnored(ignoredFilePaths, ignoredNamespaces);

	ClassDeclsCallback classCallback;
	FeildDeclsCallback fieldCallback;
	MethodDeclsCallback methodCallback;
	MethodVarsCallback methodVarCallback;
	MatchFinder Finder;
	Finder.addMatcher(ClassDeclMatcher, &classCallback);
	Finder.addMatcher(FieldDeclMatcher, &fieldCallback); 
	Finder.addMatcher(MethodDeclMatcher, &methodCallback);
	Finder.addMatcher(MethodVarMatcher, &methodVarCallback);
	int result = Tool->run(newFrontendActionFactory(&Finder).get());
	delete Tool;
	if (OptionsParser)
		delete OptionsParser;
	return result;
}
