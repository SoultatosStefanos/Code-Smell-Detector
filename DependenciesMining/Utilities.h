#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4146)

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <iostream>

using namespace clang;
using namespace clang::ast_matchers;
using namespace llvm;

template<typename T> void PrintLocation(T d, const MatchFinder::MatchResult& result);

std::string GetFullTemplateArgsName(const RecordDecl* d);
std::string GetFullStructureName(const RecordDecl* d);
std::string GetFullMethodName(const CXXMethodDecl* d);

bool isStructureOrStructurePointerType(clang::QualType type); 