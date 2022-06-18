#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4146)

#include <iostream>
#include "SymbolTable.h"
#include "Incremental.h"
#include "../Ignored/Ignored.h"
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
using namespace incremental;

namespace dependenciesMining {

	extern SymbolTable structuresTable;
	extern Sources parsedFiles;
	extern std::string currFile;

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
		inline static int scope_depth;
		inline static int scope_max_depth;
	};

	class MethodVarsCallback : public MatchFinder::MatchCallback {
	public:
		virtual void run(const MatchFinder::MatchResult& result);
	};

	// ----------------------------------------------------------------------------------

	std::unique_ptr<ClangTool> CreateClangTool(const char* cmpDBPath, std::string& errorMsg); 	// From a compile_commands.json, returns nullptr on error.
	std::unique_ptr<ClangTool> CreateClangTool(const char* cmpDBPath);							// From a compile_commands.json, returns nullptr on error.
	std::unique_ptr<ClangTool> CreateClangTool(const std::vector<std::string>& srcs);			// From a collection of file paths, returns nullptr on error.

	void SetIgnoredRegions(const char* filesPath = "", const char* namespacesPath = "");

	int MineArchitecture(ClangTool& tool); // Requires ignored regions to hav been set.

	void GetMinedFiles(ClangTool& tool, std::vector<std::string>& srcs, std::vector<std::string>& headers); // Ignores set ignored regions.

} // dependenciesMining
