#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cassert>
#include <Vector>
#include "STVisitor.h"

#define ID_T int64_t 

namespace DependenciesMining {

	class SymbolTable;
	class Structure;
	class Definition; 
	class Method;

	enum class StructureType {
		Undefined = -1,
		Class,
		Struct,
		TemplateDefinition,
		TemplateFullSpecialization,
		TemplateInstatiationSpecialization,
		TemplatePartialSpecialization
	};


	enum class MethodType {
		Undefined = -1,
		Constructor,
		Destructor,
		UserMethod,
		TemplateDefinition,
		TemplateFullSpecialization,
		TemplateInstatiationSpecialization
	};


	enum class ClassType : char {
		Undefined = -1,
		Structure,
		Method, 
		Definition
	};


	class SourceInfo {
	private:
		std::string fileName = "";
		int line = -1;
		int column = -1;
	public:
		SourceInfo() = default;
		SourceInfo(const std::string& fileName, int line, int column) : fileName(fileName), line(line), column(column) {};
		std::string GetFileName() const;
		int GetLine() const;
		int GetColumn() const;
		std::string toString() const;
		void SetFileName(const std::string& name);
		void SetLine(int line);
		void SetColumn(int column);
		void SetTotalLocation(const std::string& fileName, int line, int column);

		bool  operator<(SourceInfo const& loc) const;
		bool  operator<=(SourceInfo const& loc) const;
		bool  operator>(SourceInfo const& loc) const;
		bool  operator>=(SourceInfo const& loc) const;
	};


	class Symbol {
	protected:
		ID_T id = -1;
		std::string name;
		std::string nameSpace = "";
		SourceInfo srcInfo;
		ClassType classType = ClassType::Undefined;

	public:
		Symbol() = default; 
		Symbol(ClassType classType) : classType(classType) {};
		Symbol(ID_T id, const std::string& name, const std::string& nameSpace = "", ClassType classType = ClassType::Undefined, const std::string& fileName = "", int line = -1, int column = -1) 
			: id(id), name(name), classType(classType), srcInfo(SourceInfo(fileName, line, column)) {};

		virtual ID_T GetID() const;
		virtual std::string GetName() const;
		virtual ClassType GetClassType() const;
		virtual std::string GetClassTypeAsString() const;
		virtual const SourceInfo& GetSourceInfo() const;
		virtual std::string GetNamespace() const;

		virtual void SetID(const ID_T id);
		virtual void SetName(const std::string& name);
		virtual void SetClassType(const ClassType& type);
		virtual void SetSourceInfo(const SourceInfo& info);
		virtual void SetSourceInfo(const std::string& fileName, int line, int column);
		virtual void SetNamespace(const std::string& nameSpace);
	};


	class SymbolTable {
	private:
		std::unordered_map<ID_T, Symbol*> byID;
		std::unordered_map<std::string, std::list<Symbol*>> byName;
	public:
		Symbol* Install(ID_T id, const std::string& name, const ClassType& type = ClassType::Structure);		// TO FIX
		Symbol* Install(ID_T id, const Symbol& symbol);
		Symbol* Install(ID_T id, const Structure& symbol);
		Symbol* Install(ID_T id, const Method& symbol);
		Symbol* Install(ID_T id, const Definition& symbol);
		Symbol* Install(ID_T id, Symbol* symbol);
		Symbol* Lookup(ID_T id);
		Symbol* Lookup(const std::string& name);
		const Symbol* Lookup(ID_T id) const;
		const Symbol* Lookup(const std::string& name) const;

		void Print();
		void Accept(STVisitor* visitor);

		using iterator = std::unordered_map <ID_T, Symbol*>::iterator;
		using const_iterator = std::unordered_map <ID_T, Symbol*>::const_iterator;

		iterator begin() { return byID.begin(); }

		const_iterator begin() const { return byID.begin(); }

		iterator end() { return byID.end(); }

		const_iterator end() const { return byID.end(); }

	};


	template<typename Parent_T> class Template {
	private:
		Parent_T* parent = nullptr;
		SymbolTable arguments;

	public:
		Template() = default;

		Parent_T* GetParent() const;
		SymbolTable GetArguments() const;
		void SetParent(Parent_T* structure); 
		Symbol* InstallArguments(ID_T id, Structure* structure);
	};


	class Definition : public Symbol {
	private:
		Structure* type = nullptr;

	public:
		Definition() : Symbol(ClassType::Definition) {};
		Definition(ID_T id, const std::string& name, const std::string& nameSpace = "", Structure* type = nullptr)
			: Symbol(id, name, nameSpace, ClassType::Definition), type(type) {};
		Definition(ID_T id, const std::string& name, const std::string& nameSpace, Structure* type, const std::string& fileName, int line, int column)
			: Symbol(id, name, nameSpace, ClassType::Definition, fileName, line, column), type(type) {};

		const Structure* GetType() const;
		void SetType(Structure* structure);
	};


	class Method : public Symbol {
	public:
		class Member {
		private:
			std::string name;
			SourceInfo locEnd;
			Structure* type;

		public:
			Member() = default;
			Member(const std::string& name, Structure* type, SourceInfo locEnd) : name(name), type(type), locEnd(locEnd) {};
			std::string GetName() const;
			SourceInfo GetLocEnd() const;
			Structure* GetType() const;

			void SetName(const std::string& name);
			void SetLocEnd(const SourceInfo& locEnd);
			void SetType(Structure* type);
		};

		class MemberExpr {
			std::string expr;
			SourceInfo srcInfo;									// srcInfo: expr position on src code		
			SourceInfo locEnd;									// to store the longer expr (as string)
			std::vector<Member> members;
		public:
			MemberExpr() = default;
			MemberExpr(std::string expr, SourceInfo locEnd, std::string fileName, int line, int column) : expr(expr), locEnd(locEnd), srcInfo(SourceInfo(fileName, line, column)) {};
			std::string GetExpr() const;
			std::vector<Member> GetMembers() const;
			SourceInfo GetLocEnd() const;
			SourceInfo GetSourceInfo() const;
			void SetExpr(std::string expr);
			void SetLocEnd(SourceInfo locEnd);
			void InsertMember(Member member);
		};

	private:
		MethodType methodType = MethodType::Undefined;
		Structure* returnType = nullptr;
		Template<Method> templateInfo;								// Template Parent is *not* used
		SymbolTable arguments;
		SymbolTable definitions;
		std::unordered_map<std::string, MemberExpr> memberExprs;	// <location, MemberExpr>
	public:
		Method() : Symbol(ClassType::Method) {};
		Method(ID_T id, const std::string& name, const std::string& nameSpace = "") : Symbol(id, name, nameSpace, ClassType::Method) {};
		Method(ID_T id, const std::string& name, const std::string& nameSpace, const std::string& fileName, int line, int column)
			: Symbol(id, name, nameSpace, ClassType::Method, fileName, line, column) {};

		MethodType GetMethodType() const;
		std::string GetMethodTypeAsString() const;
		Structure* GetReturnType() const;

		SymbolTable GetArguments() const;
		SymbolTable GetDefinitions() const;
		SymbolTable GetTemplateArguments() const;
		std::unordered_map<std::string, MemberExpr> GetMemberExpr() const;

		void SetMethodType(const MethodType& type);
		void SetReturnType(Structure* structure);
		void SetTemplateParent(Method* structure);

		void InstallArg(ID_T id, const Definition& definition);
		void InstallDefinition(ID_T id, const Definition& definition);
		void InstallTemplateSpecializationArguments(ID_T id, Structure* structure);

		void InsertMemberExpr(MemberExpr const& memberExpr, Member const& member, const std::string& locBegin);
		void UpdateMemberExpr(MemberExpr const& memberExpr, const std::string& locBegin);

		bool IsConstructor () const;
		bool IsDestructor() const;
		bool IsUserMethod() const;
		bool IsTemplateDefinition() const;
		bool IsTemplateFullSpecialization() const;
		bool IsTemplateInstatiationSpecialization() const;
	};


	class Structure : public Symbol {
	private:
		
		StructureType structureType = StructureType::Undefined;
		Template<Structure> templateInfo;
		Structure* nestedParent = nullptr;
		SymbolTable methods; 
		SymbolTable fields; 
		SymbolTable bases;
		SymbolTable contains; 
		SymbolTable friends;	// About Structures: Key->structureID, Value->Structure*
								// About Methods: Key->methodID, Value->Structure* (the parent Class which owns this method)
	public:
		Structure() : Symbol(ClassType::Structure) {};
		Structure(ID_T id, const std::string& name, const std::string& nameSpace = "", StructureType structureType = StructureType::Undefined)
			: Symbol(id, name, nameSpace, ClassType::Structure), structureType(structureType) {};
		Structure(ID_T id, const std::string& name, const std::string& nameSpace, StructureType structureType, const std::string& fileName, int line, int column)
			: Symbol(id, name, nameSpace, ClassType::Structure, fileName, line, column), structureType(structureType) {};
		Structure(const Structure& s); 
		
		StructureType GetStructureType() const;
		std::string GetStructureTypeAsString() const;
		Structure* GetTemplateParent() const;
		Structure* GetNestedParent() const;

		SymbolTable GetMethods() const; 
		SymbolTable GetFields() const; 
		SymbolTable GetBases() const; 
		SymbolTable GetContains() const;
		SymbolTable GetFriends() const;
		SymbolTable GetTemplateArguments() const; 

		void SetStructureType(const StructureType& structureType);
		void SetTemplateParent(Structure* structure);
		void SetNestedParent(Structure* structure);

		Symbol* LookupMethod(ID_T id);

		Symbol* InstallMethod(ID_T id, const Method& method);
		Symbol* InstallField(ID_T id, const Definition& definition);
		Symbol* InstallBase(ID_T id, Structure* structure);
		Symbol* InstallNestedClass(ID_T id, Structure* structure);
		Symbol* InstallFriend(ID_T id, Structure* structure);
		Symbol* InstallTemplateSpecializationArguments(ID_T id, Structure* structure);

		bool IsTemplateDefinition() const;
		bool IsTemplateFullSpecialization() const;
		bool IsTemplateInstatiationSpecialization() const;
		bool IsTemplatePartialSpecialization() const;
		bool IsTemplate() const;
		bool IsNestedClass() const;
	};
}