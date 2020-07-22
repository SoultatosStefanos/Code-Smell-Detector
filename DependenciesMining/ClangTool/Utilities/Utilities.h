#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4146)

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <iostream>

#define ID_T std::string

using namespace clang;
using namespace clang::ast_matchers;
using namespace llvm;

namespace dependenciesMining {

	template<typename T>
	void PrintLocation(T d, const MatchFinder::MatchResult& result);

	bool isStructureOrStructurePointerType(const clang::QualType& type);

	template<typename Callback, typename ...Types>
	void TemplateArgsVisit(const TemplateArgument& templateArg, const Callback& callback, Types... args);
	clang::QualType GetTemplateArgType(const clang::TemplateArgument& arg);
	void AppendTemplateArgNameCallback(const TemplateArgument& templateArg, std::string* args);
	std::string GetInnerTemplateArgs(const RecordDecl* d);
	std::string GetFullStructureName(const RecordDecl* d);
	std::string GetFullMethodName(const CXXMethodDecl* d);

	std::string GetFullNamespaceName(const RecordDecl* d);

	ID_T GetIDfromDecl(const RecordDecl* d);
	ID_T GetIDfromDecl(const CXXMethodDecl* d);
	ID_T GetIDfromDecl(const FieldDecl* d);
	ID_T GetIDfromDecl(const VarDecl* d);

	// ---------------------------- template function definition ---------------------------------
	template<typename Callback, typename ...Types>
	void TemplateArgsVisit(const TemplateArgument& templateArg, const Callback& callback, Types... args) {
		if (templateArg.getKind() == templateArg.Pack) {
			auto argArray = templateArg.getPackAsArray();
			for (auto it : argArray) {
				TemplateArgsVisit(it, callback, args...);
			}
		}
		/*if (templateArg.getKind() == templateArg.TemplateExpansion) {
			templateArg.getExpansionType()
		}*/
		else {
			callback(templateArg, args...);
		}
	}

	template<typename T> void PrintLocation(T d, const MatchFinder::MatchResult& result) {
		auto& sm = *result.SourceManager;
		auto loc = d->getLocation().printToString(sm);
		std::cout << "\t" << loc << "\n\n";
	}

}