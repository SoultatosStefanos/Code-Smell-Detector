#include "DependenciesMining.h"

using namespace DependenciesMining;

StructuresTable DependenciesMining::structuresTable;

DeclarationMatcher ClassDeclMatcher = anyOf(cxxRecordDecl(isClass()).bind("ClassDecl"), cxxRecordDecl(isStruct()).bind("StructDecl"));
DeclarationMatcher FieldDeclMatcher = fieldDecl().bind("FeildDecl");
DeclarationMatcher MethodDeclMatcher = cxxMethodDecl().bind("MethodDecl");

//DeclarationMatcher MethodDeclMatcher2 = cxxMethodDecl(hasBody((hasType(recordDecl())))).bind("MethodDecl2");
//DeclarationMatcher MethodDeclMatcher2 = cxxMethodDecl(hasBody(compoundStmt())).bind("MethodDecl2");
//DeclarationMatcher MethodDeclMatcher2 = varDecl(hasParent(cxxMethodDecl())).bind("MethodDecl2");	
DeclarationMatcher MethodDeclMatcher2 = varDecl().bind("MethodDecl2");	
 
template<typename T> void PrintLocation(T d, const MatchFinder::MatchResult& result) {
	auto& sm = *result.SourceManager;
	auto loc = d->getLocation().printToString(sm);
	std::cout << "\t" << loc << "\n\n";
}

std::string GetFullStructureName(const RecordDecl* d) {
	if (d->getKind() == d->ClassTemplateSpecialization) {
		auto temp = (ClassTemplateSpecializationDecl*)d;
		auto qualifiedName = d->getQualifiedNameAsString();
		auto name = d->getNameAsString();
		std::string args = "<";
		for (int i = 0; i < temp->getTemplateArgs().size(); ++i) {
			if (args != "<")
				args += ", ";
			if(temp->getTemplateArgs()[i].getAsType()->isStructureOrClassType())
				args += temp->getTemplateArgs()[i].getAsType()->getAsCXXRecordDecl()->getQualifiedNameAsString();
			else
				args += temp->getTemplateArgs()[i].getAsType().getAsString();
		}
		args += ">";
		return (qualifiedName + args + "::" + name);
	}
	else {
		return d->getQualifiedNameAsString();
	}
}


std::string GetFullMethodName(const CXXMethodDecl* d) {
	std::string str = d->getType().getAsString();
	std::size_t pos = str.find("(");
	std::string argList = str.substr(pos);
	return d->getQualifiedNameAsString() + argList; 
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

	if (d->isInStdNamespace() || !d->hasDefinition()) return;	// ignore std && declarations
	if (d->getDescribedClassTemplate() || d->getKind() == d->ClassTemplatePartialSpecialization) return; // ignore templates 
	
	// Template Specialization 
	if (d->getKind() == d->ClassTemplateSpecialization) {
		structure.SetStructureType(StructureType::TemplateSpecialization);
	}

	//std::cout << "Name: " << d->getNameAsString() << "\tQualifiedName: " << d->getQualifiedNameAsString() << "\nMy Name: " << GetFullStructureName(d) << "\n\n";
	structure.SetName(GetFullStructureName(d));
	
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
		std::string baseName = GetFullStructureName(it.getType()->getAsCXXRecordDecl());
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
			if (!parentStructure) continue;
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
	const auto* parent = d->getParent();
	//llvm::outs() << parent->getDeclKind() << parent->getDeclKindName() << "\n"; 

	if (parent && (parent->getDeclKind() == 33 || parent->getDeclKind() == 34)) {
		std::string parentName = GetFullStructureName((RecordDecl*)parent);
		Structure* parentStructure = structuresTable.Get(parentName);
		if (!parentStructure) return;																		// Templates
		structure.SetContained(parentStructure);
	}
	
	
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
			if (!typeStructure)
				typeStructure = structuresTable.Insert(typeName);
			if (!parentStructure)																			 // Templates
				return;							

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
		if (!parentStructure) return;
		//llvm::outs() << "Method:  " << GetFullMethodName(d) << "\n\tParent: " << parentName << "\n\n";
		Method method(GetFullMethodName(d));
		parentStructure->InsertMethod(GetFullMethodName(d), method);

		// Arguments
		/*unsigned numParams = d->getNumParams();
		for (unsigned i = 0; i < numParams; ++i) {
			std::cout << d->getParamDecl(i)->getType().getAsString() << " -- ";
		}
		std::cout << std::endl;*/		
	}
}

void MethodDeclsCallback2::run(const MatchFinder::MatchResult& Result) {
	if (const VarDecl* d = Result.Nodes.getNodeAs<VarDecl>("MethodDecl2")) {
		auto parent = d->getParentFunctionOrMethod();

		//d->isFunctionOrMethodVarDecl()-> like isLocalVarDecl() but excludes variables declared in blocks?.
		if(d->isLocalVarDeclOrParm() && parent && parent->getDeclKind() == 52) {	// including params
		//if(d->isFunctionOrMethodVarDecl() && parent->getDeclKind() == 52){		// excluding params			
			if (d->isLocalVarDecl()) {
				llvm::outs() << "Var:   " << d->getName() << "   \n" << "  " << d->getType().getAsString() << "  \n";
			}
			else {
				llvm::outs() << "Arg:   " << d->getName() << "   \n" << "  " << d->getType().getAsString() << "  \n";
			}

			// get parent (class)

			auto parentClass = (CXXRecordDecl*)parent->getParent();
			llvm::outs() << parentClass->getName() << "\n";

			/*llvm::outs() << d->getDeclKindName() << "\n"; 
			llvm::outs() << ((CXXMethodDecl*)parent)->getName() << "\n";
			llvm::outs() << d->isFunctionOrMethodVarDecl() << "\n";
			std::cout << "-----------------------------------\n";*/
		}
		/*else {
			llvm::outs() << "!!!!!!!!!!!!!!! Trash:   " << d->getName() << "   \n";
		}*/
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
	//MethodDeclsCallback2 methodCallback2;
	MatchFinder Finder;
	Finder.addMatcher(ClassDeclMatcher, &classCallback);
	Finder.addMatcher(FieldDeclMatcher, &fieldCallback); 
	Finder.addMatcher(MethodDeclMatcher, &methodCallback);
	//Finder.addMatcher(MethodDeclMatcher2, &methodCallback2);
	int result = Tool.run(newFrontendActionFactory(&Finder).get());
	return result;
}
