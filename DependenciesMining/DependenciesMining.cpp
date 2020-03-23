#include "DependenciesMining.h"
#include "Utilities.h"

using namespace DependenciesMining;

StructuresTable DependenciesMining::structuresTable;

DeclarationMatcher ClassDeclMatcher = anyOf(cxxRecordDecl(isClass()).bind("ClassDecl"), cxxRecordDecl(isStruct()).bind("StructDecl"));
DeclarationMatcher FieldDeclMatcher = fieldDecl().bind("FeildDecl");
DeclarationMatcher MethodDeclMatcher = cxxMethodDecl().bind("MethodDecl");

//DeclarationMatcher MethodDeclMatcher2 = cxxMethodDecl(hasBody((hasType(recordDecl())))).bind("MethodDecl2");
//DeclarationMatcher MethodDeclMatcher2 = cxxMethodDecl(hasBody(compoundStmt())).bind("MethodDecl2");
//DeclarationMatcher MethodDeclMatcher2 = varDecl(hasParent(cxxMethodDecl())).bind("MethodDecl2");	
DeclarationMatcher MethodVarMatcher = varDecl().bind("MethodVar");	

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
void FeildDeclsCallback::run(const MatchFinder::MatchResult& Result) {
	if (const FieldDecl* d = Result.Nodes.getNodeAs<FieldDecl>("FeildDecl")) {
		if (!d->getType()->isStructureOrClassType()) {
			if (d->getType()->isPointerType()) {
				if(!d->getType()->getPointeeType()->isStructureOrClassType())
					return;
			}
			else
				return;
		}
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
void MethodDeclsCallback::run(const MatchFinder::MatchResult& Result) {
	if (const CXXMethodDecl* d = Result.Nodes.getNodeAs<CXXMethodDecl>("MethodDecl")) {
		const RecordDecl* parent = d->getParent();
		std::string parentName = GetFullStructureName(parent); 
		Structure* parentStructure = structuresTable.Get(parentName);
		//llvm::outs() << "Method:  " << GetFullMethodName(d) << "\n\tParent: " << parentName << "\n\n";
		Method method(GetFullMethodName(d));
		parentStructure->InsertMethod(GetFullMethodName(d), method);
	}
}

void MethodVarsCallback::run(const MatchFinder::MatchResult& Result) {
	if (const VarDecl* d = Result.Nodes.getNodeAs<VarDecl>("MethodVar")) {
		auto parentMethod = d->getParentFunctionOrMethod();
		if(d->isLocalVarDeclOrParm() && parentMethod && parentMethod->getDeclKind() == d->CXXMethod) {	// including params
		//if(d->isFunctionOrMethodVarDecl() && parentMethod->getDeclKind() == d->CXXMethod){		// excluding params	- d->isFunctionOrMethodVarDecl()-> like isLocalVarDecl() but excludes variables declared in blocks?.		
			if (!d->getType()->isStructureOrClassType()) {
				if (d->getType()->isPointerType()) {
					if (!d->getType()->getPointeeType()->isStructureOrClassType())
						return;
				}
				else
					return;
			}
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
	MatchFinder Finder;
	Finder.addMatcher(ClassDeclMatcher, &classCallback);
	Finder.addMatcher(FieldDeclMatcher, &fieldCallback); 
	Finder.addMatcher(MethodDeclMatcher, &methodCallback);
	Finder.addMatcher(MethodVarMatcher, &methodVarCallback);
	int result = Tool.run(newFrontendActionFactory(&Finder).get());
	return result;
}
