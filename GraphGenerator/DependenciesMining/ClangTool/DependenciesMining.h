#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4146)

#include <iostream>
#include "SymbolTable.h"
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
	
	// ----------------------------------------------------------------------------------

	class ClassDeclsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);
	};

	class FeildDeclsCallback : public MatchFinder::MatchCallback {
	private:
		void installFundamentalField(const MatchFinder::MatchResult& result);
	public:
		virtual void run(const MatchFinder::MatchResult& result);
	};

	class MethodDeclsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);

		class FindMemberExprVisitor : public RecursiveASTVisitor<FindMemberExprVisitor> {
		public:
			bool VisitMemberExpr(MemberExpr* expr);
			bool TraverseStmt(Stmt* stmt);
		};
	private: 
		friend FindMemberExprVisitor;
		inline static Method* currentMethod = nullptr;
		inline static SourceManager* sm = nullptr;
		inline static int literal_count;
		inline static int statement_count;
		inline static int loop_count;
		inline static int branch_count;
	};

	class MethodVarsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);
	};

	// ----------------------------------------------------------------------------------

	std::unique_ptr<CompilationDatabase> LoadCompilationDatabase(const char*);
	int CreateClangTool(const char* cmpDBPath, std::vector<std::string> srcs, const char* ignoredFilePaths, const char* ignoredNamespaces);

}
