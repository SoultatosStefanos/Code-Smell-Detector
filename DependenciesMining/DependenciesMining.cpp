#include "DependenciesMining.h"
#include "Utilities.h"

using namespace DependenciesMining;

StructuresTable DependenciesMining::structuresTable;

DeclarationMatcher ClassDeclMatcher = anyOf(cxxRecordDecl(isClass()).bind("ClassDecl"), cxxRecordDecl(isStruct()).bind("StructDecl"));
DeclarationMatcher FieldDeclMatcher = fieldDecl().bind("FeildDecl");
DeclarationMatcher MethodDeclMatcher = cxxMethodDecl().bind("MethodDecl");
DeclarationMatcher MethodVarMatcher = varDecl().bind("MethodVar");
//StatementMatcher MemberMatcher = memberExpr().bind("Member");
//StatementMatcher MemberMatcher = memberExpr(hasObjectExpression(hasType(cxxRecordDecl()))).bind("Member");


template<typename T> void PrintLocation(T d, const MatchFinder::MatchResult& result) {
	auto& sm = *result.SourceManager;
	auto loc = d->getLocation().printToString(sm);
	std::cout << "\t" << loc << "\n\n";
}


// Handle all the Classes and Structs and the Bases
void ClassDeclsCallback::run(const MatchFinder::MatchResult& result) {
	const CXXRecordDecl* d;
	Structure structure;
	if ((d = result.Nodes.getNodeAs<CXXRecordDecl>("ClassDecl"))) {
		structure.SetStructureType(StructureType::Class);
	}
	else if ((d = result.Nodes.getNodeAs<CXXRecordDecl>("StructDecl"))) {
		structure.SetStructureType(StructureType::Struct);
	}
	else {
		assert(0);
	}

	if (d->isInStdNamespace() || !d->hasDefinition()) {
		return;														// ignore std && declarations
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
			structure.SetStructureType(StructureType::TemplateInstatiationSpecialization);
		else 
			structure.SetStructureType(StructureType::TemplateFullSpecialization);
	}

	//std::cout << "Name: " << d->getNameAsString() << "\tQualifiedName: " << d->getQualifiedNameAsString() << "\nMy Name: " << GetFullStructureName(d) << "\n\n";
	
	structure.SetName(GetFullStructureName(d));

	// Templates
	if (d->getKind() == d->ClassTemplateSpecialization || d->getKind() == d->ClassTemplatePartialSpecialization) {
		// Template parent
		std::string parentName;
		if (structure.IsTemplateInstatiationSpecialization()) {
			auto* parent = d->getTemplateInstantiationPattern();
			parentName = GetFullStructureName(parent);
		}
		else {
			parentName = d->getQualifiedNameAsString();				// mporei kai na mhn xreiazetai na orisw parent mia kai to full specialization einai mia diaforetikh class (partial specialization einai san new template)
		}
		Structure* templateParent = structuresTable.Get(parentName);
		if (!templateParent)
			templateParent = structuresTable.Insert(parentName);
		structure.SetTemplateParent(templateParent);

		//Template Arguments
		auto temp = (ClassTemplateSpecializationDecl*)d;
		for (unsigned i = 0; i < temp->getTemplateArgs().size(); ++i) {
			if (temp->getTemplateArgs()[i].getAsType()->isStructureOrClassType()) {
				std::string argName = GetFullStructureName(temp->getTemplateArgs()[i].getAsType()->getAsCXXRecordDecl());
				Structure* arg = structuresTable.Get(argName);
				structure.InsertTemplateSpecializationArguments(argName, arg);
			}
		}
	}
	
	// Namespace
	auto enclosingNamespace = d->getEnclosingNamespaceContext();
	std::string fullEnclosingNamespace = "";
	while (enclosingNamespace->isNamespace()) {
		fullEnclosingNamespace = ((NamespaceDecl*)enclosingNamespace)->getNameAsString() + "::" + fullEnclosingNamespace;
		enclosingNamespace = enclosingNamespace->getParent()->getEnclosingNamespaceContext();
	}
	structure.SetEnclosingNamespace(fullEnclosingNamespace);

	// Bases
	for(auto it : d->bases()){
		auto* baseRecord = it.getType()->getAsCXXRecordDecl();	
		if (baseRecord == nullptr)										// otan base einai template or partial specialization Ignored
			continue;
		std::string baseName = GetFullStructureName(baseRecord);	
		Structure* base = structuresTable.Get(baseName);
		if (!base)
			base = structuresTable.Insert(baseName);
		structure.InsertBase(baseName, base);
	}

	// Friends 
	for (auto it : d->friends()) {
		auto type = it->getFriendType();
		if (type) {																							// Classes
			std::string parentName = GetFullStructureName(type->getType()->getAsCXXRecordDecl()) ;
			Structure* parentStructure = structuresTable.Get(parentName);
			if (!parentStructure) continue;								// templates
			structure.InsertFriend(parentName, parentStructure);
		}
		else {																								// Methods			
			auto decl = it->getFriendDecl();
			if (decl->getKind() == it->CXXMethod) {
				std::string methodName = GetFullMethodName((CXXMethodDecl*)decl);
				auto parentClass = ((CXXMethodDecl*)decl)->getParent();
				std::string parentName = GetFullStructureName(parentClass);
				Structure* parentStructure = structuresTable.Get(parentName);
				if (!parentStructure) continue;
				structure.InsertFriend(methodName, parentStructure);
			}
		}
	}

	// Members
	if (d->isCXXClassMember()) {
		const auto* parent = d->getParent();
		assert(parent);
		std::string parentName = GetFullStructureName((RecordDecl*)parent);
		if (parentName != structure.GetName()) {
			Structure* parentStructure = structuresTable.Get(parentName);
			auto inst = d->getInstantiatedFromMemberClass();
			structure.SetNestedParent(parentStructure);
			parentStructure->InsertNestedClass(structure.GetName(), structuresTable.Insert(structure.GetName()));
		}
	}
	//std::cout << "\n";
	structuresTable.Insert(structure.GetName(), structure);
}

// Hanlde all the Fields in classes/structs
void FeildDeclsCallback::run(const MatchFinder::MatchResult& result) {
	if (const FieldDecl* d = result.Nodes.getNodeAs<FieldDecl>("FeildDecl")) {
		if (!isStructureOrStructurePointerType(d->getType()))
			return; 

		const RecordDecl* parent = d->getParent();
		if (parent->isClass() || parent->isStruct()) {
			std::string parentName = GetFullStructureName(parent);
			std::string typeName;
			if(d->getType()->isPointerType())
				typeName = GetFullStructureName(d->getType()->getPointeeType()->getAsCXXRecordDecl());
			else
				typeName = GetFullStructureName(d->getType()->getAsCXXRecordDecl());
			
			Structure* parentStructure = structuresTable.Get(parentName);
			Structure* typeStructure = structuresTable.Get(typeName);
			if (parentStructure->IsTemplateInstatiationSpecialization())		// insertion speciallization inherite its dependencies from the parent template
				return;
			if (!typeStructure)
				typeStructure = structuresTable.Insert(typeName);
			
			//llvm::outs() << "Field:  " << d->getName() << "\tQualified Name: " << d->getQualifiedNameAsString() << "\n\tParent: " << parentName << "   " << typeName << "\n";

			Definition field(d->getQualifiedNameAsString(), typeStructure);
			parentStructure->InsertField(d->getQualifiedNameAsString(), field);	
		} 
	}
}

// Handle all the Methods
void MethodDeclsCallback::run(const MatchFinder::MatchResult& result) {
	if (const CXXMethodDecl* d = result.Nodes.getNodeAs<CXXMethodDecl>("MethodDecl")) {
		const RecordDecl* parent = d->getParent();
		std::string parentName = GetFullStructureName(parent);
		Structure* parentStructure = structuresTable.Get(parentName);
		//llvm::outs() << "Method:  " << GetFullMethodName(d) << "\n\tParent: " << parentName << "\n\n";
		Method method(GetFullMethodName(d));

		//Return
		auto returnType = d->getReturnType();
		if (isStructureOrStructurePointerType(returnType)) {
			std::string typeName;
			if (returnType->isPointerType())
				typeName = GetFullStructureName(returnType->getPointeeType()->getAsCXXRecordDecl());
			else
				typeName = GetFullStructureName(returnType->getAsCXXRecordDecl());
			Structure* typeStructure = structuresTable.Get(typeName);
			if (!typeStructure)
				typeStructure = structuresTable.Insert(typeName);
			method.SetReturnType(typeStructure);
		}

		currentMethod = parentStructure->InsertMethod(GetFullMethodName(d), method);

		// Body
		auto body = d->getBody();
		FindMemberClassVisitor visitor; 
		std::cout << GetFullMethodName(d) <<"------------------\n";
		visitor.TraverseStmt(body);
		std::cout << "------------------\n\n";
		currentMethod = nullptr;
	}
}



bool MethodDeclsCallback::FindMemberClassVisitor::VisitMemberExpr(MemberExpr* memberExpr) {
	auto decl = memberExpr->getMemberDecl();
	auto type = decl->getType();
	auto base = memberExpr->getBase();
	if (base->getStmtClass() == memberExpr->CXXThisExprClass) { // ignore class' fields
		std::cout << "~~~~~~~~~~~~~~~this\n\n";
		return true;
	}
	if (!isStructureOrStructurePointerType(type)) {
		if (decl->getKind() == decl->CXXMethod) {
			CXXMethodDecl* methodDecl = (CXXMethodDecl*)decl;
			type = methodDecl->getReturnType();
			if (!isStructureOrStructurePointerType(type)) {
				std::cout << "~~~~~~~~~~~~~~~method\n\n";
				return true;
			}
		}
		else
			return true;
	}
	std::string typeName;
	if (type->isPointerType())
		typeName = GetFullStructureName(type->getPointeeType()->getAsCXXRecordDecl());
	else
		typeName = GetFullStructureName(type->getAsCXXRecordDecl());
	Structure* typeStructure = structuresTable.Get(typeName);
	if (!typeStructure)
		typeStructure = structuresTable.Insert(typeName);

	llvm::outs() << "Decl Type: " << typeName << "\n";

	MethodDeclsCallback::currentMethod->InsertMemberExpr(typeName, typeStructure);

	/*if (base->getStmtClass() == memberExpr->DeclRefExprClass) {
		// apo var tou method
	}
	else if (base->getStmtClass() == memberExpr->MemberExprClass) {
		// apo field ths class
	}
	else if (base->getStmtClass() == memberExpr->CXXThisExprClass) {
		// apo this
		std::cout << "~~~~~~~~~~~~~~~this\n\n";
		return true;										
	}
	else {
		llvm::outs() << base->getStmtClassName() << " !!!!!!!!!!!\n";
	}*/
	llvm::outs() << "Kind: " << decl->getDeclKindName() << "\n";
	llvm::outs() << "Name: " << decl->getNameAsString() << "\n";
	
	std::cout << "~~~~~~~~~~~~~~~\n\n";
	return true;
}

/*
void MemberOnMethodsCallback::run(const MatchFinder::MatchResult& result) {
	if (const Stmt* d = result.Nodes.getNodeAs<Stmt>("Member")) {
		// ---------------------------------------------------------------
		if (d->getStmtClass() == d->CXXMemberCallExprClass) {
			auto memberExpr = static_cast<const CXXMemberCallExpr*>(d);
			auto decl = memberExpr->getCalleeDecl();
			auto decl2 = memberExpr->getRecordDecl();
			if (decl2->getParentFunctionOrMethod())
				std::cout << "Method's Method parent\n\n";
		}
		else if (d->getStmtClass() == d->MemberExprClass) {
		//	auto memberExpr = static_cast<const MemberExpr*>(d);
		//	auto decl = memberExpr->getMemberDecl();
		//	auto base = memberExpr->getBase();
		} 
		else 
			assert(0);
		//------------------------------------------------------------------

		auto memberExpr = static_cast<const MemberExpr*>(d);
		memberExpr->getDependence();
		//memberExpr->getExprStmt();
		memberExpr->getMemberLoc();		/// the real location (decl location is the location where this member is declerd - useless)
		

		auto decl = memberExpr->getMemberDecl();
		auto base = memberExpr->getBase();
		auto& sm = *result.SourceManager;
		std::cout << "Member Location:\t" << memberExpr->getMemberLoc().printToString(sm) << "\n\n";
			
	//	PrintLocation(decl, result);

		auto type = decl->getType();
		if (!isStructureOrStructurePointerType(type)) {
			if (decl->getKind() == decl->CXXMethod) {
				CXXMethodDecl* methodDecl = (CXXMethodDecl*)decl;
				type = methodDecl->getReturnType();
				if (!isStructureOrStructurePointerType(type)) {
					std::cout << "~~~~~~~~~~~~~~~method\n\n";
					return;
				}
			}
			else 
				return; 
		}

		std::string typeName;
		if (type->isPointerType())
			typeName = GetFullStructureName(type->getPointeeType()->getAsCXXRecordDecl());
		else
			typeName = GetFullStructureName(type->getAsCXXRecordDecl());
		Structure* typeStructure = structuresTable.Get(typeName);
		if (!typeStructure)
			typeStructure = structuresTable.Insert(typeName);

		llvm::outs() << "Decl Type: " << typeName << "\n";

		//auto context = decl->getDeclContext();
		//if (context) {
		//	llvm::outs() << " --- Decl Context: " << context->getDeclKindName() << "\n";
		//}
		
		if (base->getStmtClass() == d->DeclRefExprClass) {
			// apo var tou method
			auto baseDecl = ((DeclRefExpr*)base)->getDecl();
			
			if (auto parentMethod = baseDecl->getParentFunctionOrMethod()) {
				auto parentMethodName = GetFullMethodName((CXXMethodDecl*)parentMethod);
				llvm::outs() << "Parent: " << parentMethod->getDeclKindName() << "   " << parentMethodName << "\n";
			}
		}
		else if (base->getStmtClass() == d->MemberExprClass) {
			// apo field ths class
			auto baseDecl = ((MemberExpr*)base)->getMemberDecl();
			if (auto parentMethod = baseDecl->getParentFunctionOrMethod()) {			// apo member expr den moy epistrefei to method
					auto parentMethodName = GetFullMethodName((CXXMethodDecl*)parentMethod);
					llvm::outs() << "!!!!!!!!!!!!Parent: " << parentMethod->getDeclKindName() << "   " << parentMethodName << "\n";
				}
		}
		else if (base->getStmtClass() == d->CXXThisExprClass) {
			// apo this
			//auto thisDecl = ((CXXThisExpr*)base);
			std::cout << "~~~~~~~~~~~~~~~this\n\n";
			return;
		}
		else {
			llvm::outs() << base->getStmtClassName() << " !!!!!!!!!!!\n";
		}
		llvm::outs() << "Kind: " << decl->getDeclKindName() << "\n";
		llvm::outs() << "Name: " << decl->getNameAsString() << "\n";			
	}
	std::cout << "~~~~~~~~~~~~~~~\n\n";
}*/

// Handle Method's Vars and Args
void MethodVarsCallback::run(const MatchFinder::MatchResult& result) {
	if (const VarDecl* d = result.Nodes.getNodeAs<VarDecl>("MethodVar")) {
		auto parentMethod = d->getParentFunctionOrMethod();
		if (d->isLocalVarDeclOrParm() && parentMethod && parentMethod->getDeclKind() == d->CXXMethod) {	// including params
		//if(d->isFunctionOrMethodVarDecl() && parentMethod->getDeclKind() == d->CXXMethod){		// excluding params	- d->isFunctionOrMethodVarDecl()-> like isLocalVarDecl() but excludes variables declared in blocks?.		
			if (!isStructureOrStructurePointerType(d->getType()))
				return;

			auto parentClass = (CXXRecordDecl*)parentMethod->getParent();
			auto parentClassName = GetFullStructureName(parentClass);
			auto parentMethodName = GetFullMethodName((CXXMethodDecl*)parentMethod);
			Structure* parentStructure = structuresTable.Get(parentClassName);
			Method* parentMethod = parentStructure->GetMethod(parentMethodName);
			std::string typeName;
			if (d->getType()->isPointerType())
				typeName = GetFullStructureName(d->getType()->getPointeeType()->getAsCXXRecordDecl());
			else
				typeName = GetFullStructureName(d->getType()->getAsCXXRecordDecl());
			Structure* typeStructure = structuresTable.Get(typeName);
			if (!typeStructure)
				typeStructure = structuresTable.Insert(typeName);
			Definition def(d->getQualifiedNameAsString(), typeStructure);
			if (d->isLocalVarDecl()) {
				parentMethod->InsertDefinition(d->getQualifiedNameAsString(), def);
			}
			else {
				parentMethod->InsertArg(d->getQualifiedNameAsString(), def);
			}
		}
	}
}
/*
	Clang Tool Creation
*/
static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nA help message for this specific tool can be added afterwards..\n");

int DependenciesMining::CreateClangTool(int argc, const char** argv, std::vector<std::string> srcs) {
	CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
	ClangTool Tool(OptionsParser.getCompilations(), srcs);
	
	ClassDeclsCallback classCallback;
	FeildDeclsCallback fieldCallback;
	MethodDeclsCallback methodCallback;
	MethodVarsCallback methodVarCallback;
	//MemberOnMethodsCallback memberCallback; 
	MatchFinder Finder;
	Finder.addMatcher(ClassDeclMatcher, &classCallback);
	Finder.addMatcher(FieldDeclMatcher, &fieldCallback); 
	Finder.addMatcher(MethodDeclMatcher, &methodCallback);
	Finder.addMatcher(MethodVarMatcher, &methodVarCallback);
	//Finder.addMatcher(MemberMatcher, &memberCallback);
	int result = Tool.run(newFrontendActionFactory(&Finder).get());
	return result;
}
