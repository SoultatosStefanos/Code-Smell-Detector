#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4146)

#include <iostream>
#include "SymbolTable.h"
//#include "SymbolTableTempl.h"
#include "../Ignored/Ignored.h";
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace llvm;
using namespace clang::tooling;

namespace dependenciesMining {
	extern SymbolTable structuresTable;
	extern std::unordered_map<std::string, Ignored*> ignored; 
	
	
	class ClassDeclsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);
	};

	class FeildDeclsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);
	};

	class MethodDeclsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);

		class FindMemberExprVisitor : public RecursiveASTVisitor<FindMemberExprVisitor> {
		public:
			bool VisitMemberExpr(MemberExpr* expr);
		};
	private: 
		friend FindMemberExprVisitor;
		inline static Method* currentMethod = nullptr;
		inline static SourceManager* sm = nullptr;
	};

	class MethodVarsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);
	};

	int CreateClangTool(int argc, const char** argv, std::vector<std::string> srcs);
}
