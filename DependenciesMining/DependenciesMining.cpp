#include "DependenciesMining.h"

DeclarationMatcher ClassDeclMatcher = anyOf(cxxRecordDecl(isClass()).bind("ClassDecl"), cxxRecordDecl(isStruct()).bind("StructDecl"));
DeclarationMatcher MethodDeclMatcher = cxxMethodDecl().bind("MethodDecl");
DeclarationMatcher FieldDeclmatcher = fieldDecl().bind("FeildDecl");

//DeclarationMatcher MethodDeclMatcher2 = cxxMethodDecl(hasBody((hasType(recordDecl())))).bind("MethodDecl2");
//DeclarationMatcher MethodDeclMatcher2 = cxxMethodDecl(hasBody(compoundStmt())).bind("MethodDecl2");
//DeclarationMatcher MethodDeclMatcher2 = varDecl(hasParent(cxxMethodDecl())).bind("MethodDecl2");	
DeclarationMatcher MethodDeclMatcher2 = varDecl().bind("MethodDecl2");	

static int count = 0; 
 
template<typename T> void printLocation(T d, const MatchFinder::MatchResult& result) {
	auto& sm = *result.SourceManager;
	auto loc = d->getLocation().printToString(sm);
	std::cout << "\t" << loc << "\n\n";
}


// Handle all the Classes and Structs and the Bases
void ClassDeclsCallback::run(const MatchFinder::MatchResult& result) {
	const CXXRecordDecl* d;
	

	if ((d = result.Nodes.getNodeAs<CXXRecordDecl>("ClassDecl")) ) {
		if (d->hasDefinition()) {
			llvm::outs() << ++count << ". Class:    " << d->getName() << "\n";
		}
	}
	else if ((d = result.Nodes.getNodeAs<CXXRecordDecl>("StructDecl"))) {
		if (d->hasDefinition())
			llvm::outs() << ++count << ". Struct:   " << d->getName() << "\n";
	}
	else {
		assert(0);
	}

	if (d->hasDefinition()) {
		for (auto it = d->bases_begin(); it != d->bases_end(); ++it) {
			std::cout << "\tBases Type: " << it->getType().getAsString() << std::endl;
		}

		//--------------------------------------------------------------------------
		if (auto templ = d->getDescribedClassTemplate()) {
	
			for (auto it = templ->spec_begin(); it != templ->spec_end(); ++it) {				// it does not handle the partial specialization
				std::cout << "\tSpecialization: " << it->getNumTemplateParameterLists() << std::endl;
			}
			//getTemplateArgs()
			//auto special = d->getTemplateSpecializationKind();

			/*if (d->getTemplateInstantiationPattern()) {
				llvm::outs() << "\t Template \n";
				auto specialization = d->getTemplateSpecializationKind();
			}*/
		}else 
		if (d->getKind() == d->ClassTemplateSpecialization) {	//  d->ClassTemplatePartialSpecialization	 ?
			std::cout << "\tTEMPLATE SPECIAL\n";
		}
		else if (d->getKind() == d->ClassTemplatePartialSpecialization) {
			std::cout << "\tTEMPLATE PARTIAL\n";
		}
		llvm::outs() << "\t" << d->getKindName() << " - " << d->getKind() << " - " << d->getTypeForDecl() << "\n";
		printLocation(d, result);
		
	}
}


// Hanlde all the Fields in classes/structs
void FeildDeclsCallback::run(const MatchFinder::MatchResult& Result) {
	if (const FieldDecl* d = Result.Nodes.getNodeAs<FieldDecl>("FeildDecl")) {
		const RecordDecl* parent = d->getParent();
		if (parent->isClass() || parent->isStruct()) {
			llvm::outs() << "Field:   " << d->getName() << "  -  Parent: " << d->getParent()->getName() << "  --  ";
			std::cout << d->getType().getAsString() << std::endl;
		}
	}
}

// Handle all the Methods - their args(and type) 
void MethodDeclsCallback::run(const MatchFinder::MatchResult& Result) {
	if (const CXXMethodDecl* d = Result.Nodes.getNodeAs<CXXMethodDecl>("MethodDecl")) {
		
		llvm::outs() << "Method:   " << d->getName() << "  ";

		// Arguments
		unsigned numParams = d->getNumParams();
		for (unsigned i = 0; i < numParams; ++i) {
			std::cout << d->getParamDecl(i)->getType().getAsString() << " -- ";
		}
		std::cout << std::endl;

		// Methods Body
		/*if(d->hasBody()){	// method definition and declaration(if it exists)			
			Stmt* body = d->getBody();
			for (auto it = body->child_begin(); it != body->child_end(); ++it) {
				std::cout << "body stmt\n";
				
			}
		}*/
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

int CreateClangTool(int argc, const char** argv, std::vector<std::string> srcs) {
	CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
	ClangTool Tool(OptionsParser.getCompilations(), srcs);

	ClassDeclsCallback classCallback;
	//FeildDeclsCallback	fieldCallback;
	//MethodDeclsCallback methodCallback;
	//MethodDeclsCallback2 methodCallback2;
	MatchFinder Finder;
	Finder.addMatcher(ClassDeclMatcher, &classCallback);
	//Finder.addMatcher(FieldDeclmatcher, &fieldCallback); 
	//Finder.addMatcher(MethodDeclMatcher, &methodCallback);
	//Finder.addMatcher(MethodDeclMatcher2, &methodCallback2);
	int result = Tool.run(newFrontendActionFactory(&Finder).get());
	return result;
}

