#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <list>
#include <cassert>
#include <vector>
#include <algorithm>
#include "json/writer.h"

#define ID_T std::string 

#define DEBUG_FRIENDLY

namespace dependenciesMining {

	class SymbolTable;
	class Structure;
	class Definition; 
	class Method;
	class STVisitor;

	enum class StructureType {
		Undefined = -1,
		Class,
		Struct,
		TemplateDefinition,
		TemplateFullSpecialization,
		TemplateInstantiationSpecialization,
		TemplatePartialSpecialization
	};

	enum class MethodType {
		Undefined = -1,
		Constructor_Trivial,
		OverloadedOperator_Trivial,
		Destructor_Trivial,
		Constructor_UserDefined,
		Destructor_UserDefined,
		UserMethod,
		OverloadedOperator_UserDefined,
		TemplateDefinition,
		TemplateFullSpecialization,
		TemplateInstantiationSpecialization
	};


	enum class ClassType {
		Undefined = -1,
		Structure,
		Method, 
		Definition
	};

	enum class AccessType {
		unknown = -1,
		_public,
		_protected,
		_private
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
		bool  operator==(SourceInfo const& loc) const;		
	};

	DEBUG_FRIENDLY inline std::ostream& operator<<(std::ostream& os, const SourceInfo& src) {
		return os << src.toString();
	}

	// ----------------------------------------------------------------------------------------

	class Symbol {
	protected:
		ID_T id = "";
		std::string name;
		std::string nameSpace = "";
		SourceInfo srcInfo;
		ClassType classType = ClassType::Undefined;
		AccessType access_type = AccessType::unknown;

		DEBUG_FRIENDLY virtual bool IsEqual(const Symbol& other) const;

	public:
		Symbol() = default; 
		Symbol(ClassType classType) : classType(classType) {};
		Symbol(const ID_T& id, const std::string& name, const std::string& nameSpace = "", ClassType classType = ClassType::Undefined, const std::string& fileName = "", int line = -1, int column = -1)
			: id(id), name(name), nameSpace(nameSpace), classType(classType), srcInfo(fileName, line, column) {};

		virtual ID_T GetID() const;
		virtual std::string GetName() const;
		virtual ClassType GetClassType() const;
		virtual std::string GetClassTypeAsString() const;
		virtual const SourceInfo& GetSourceInfo() const;
		virtual std::string GetNamespace() const;
		const char* GetAccessTypeStr() const;
		AccessType GetAccessType() const;

		virtual void SetID(const ID_T& id);
		virtual void SetName(const std::string& name);
		virtual void SetClassType(const ClassType& type);
		virtual void SetSourceInfo(const SourceInfo& info);
		virtual void SetSourceInfo(const std::string& fileName, int line, int column);
		virtual void SetNamespace(const std::string& nameSpace);
		void SetAccessType(const AccessType& access_type);

		DEBUG_FRIENDLY virtual void Output(std::ostream& os) const;
		
		DEBUG_FRIENDLY bool operator==(const Symbol& rhs) const;
	};

	DEBUG_FRIENDLY inline bool Symbol::operator==(const Symbol& rhs) const {
		// GetClassType() == rhs.GetClassType() ensures safe static casting down the line.
		// God knows why clang links only with -fno-rtti.
		return GetClassType() == rhs.GetClassType() and IsEqual(rhs); 
	}

	DEBUG_FRIENDLY inline bool operator!=(const Symbol& lhs, const Symbol& rhs) {
		return !(lhs == rhs);
	}

	DEBUG_FRIENDLY inline std::ostream& operator<<(std::ostream& os, const Symbol& s) {
		s.Output(os);
		return os;
	}

	// ----------------------------------------------------------------------------------------

	class SymbolTable {
	private:
		std::unordered_map<ID_T, Symbol*> byID;
		std::unordered_map<std::string, std::list<Symbol*>> byName;
	public:
		using Size = std::unordered_map<ID_T, Symbol*>::size_type;

		bool IsEmpty() const { return byID.empty(); }
		Size GetSize() const { return byID.size(); }

		Symbol* Install(const ID_T& id, const std::string& name, const ClassType& type = ClassType::Structure);		// TO FIX
		Symbol* Install(const ID_T& id, const Symbol& symbol);
		Symbol* Install(const ID_T& id, const Structure& symbol);
		Symbol* Install(const ID_T& id, const Method& symbol);
		Symbol* Install(const ID_T& id, const Definition& symbol);
		Symbol* Install(const ID_T& id, Symbol* symbol);
		Symbol* Lookup(const ID_T& id);
		//Symbol* Lookup(const std::string& name);
		const Symbol* Lookup(const ID_T& id) const;
		//const Symbol* Lookup(const std::string& name) const;

		void Print();
		void Print2(int level);
		Json::Value GetJsonStructure(dependenciesMining::Structure* structure);
		Json::Value GetJsonMethod(dependenciesMining::Method* method);
		Json::Value GetJsonDefinition(dependenciesMining::Definition* definition);
		void AddJsonStructure(dependenciesMining::Structure* structure, Json::Value& json_structure);
		void AddJsonMethod(dependenciesMining::Method* method, Json::Value& json_method);
		void AddJsonDefinition(dependenciesMining::Definition* definition, Json::Value& json_definition);
		void AddJsonSymbolTable(Json::Value& st);
		Json::Value GetJsonSymbolTable(void);
		void Accept(STVisitor* visitor);
		void Accept(STVisitor* visitor) const;

		using iterator = std::unordered_map <ID_T, Symbol*>::iterator;
		using const_iterator = std::unordered_map <ID_T, Symbol*>::const_iterator;

		iterator begin() { return byID.begin(); }

		const_iterator begin() const { return byID.begin(); }

		iterator end() { return byID.end(); }

		const_iterator end() const { return byID.end(); }

	};

	DEBUG_FRIENDLY inline std::ostream& operator<<(std::ostream& os, const SymbolTable& t) {
		for (const auto& [id, symbol] : t) 
			os << *symbol << '\n';
		return os;
	}

	DEBUG_FRIENDLY bool operator==(const SymbolTable& lhs, const SymbolTable& rhs); // Expensive, only for small tests

	DEBUG_FRIENDLY inline bool operator!=(const SymbolTable& lhs, const SymbolTable& rhs) {
		return	!(lhs == rhs);
	}

	// ----------------------------------------------------------------------------------------

	template<typename Parent_T> class Template {
	private:
		Parent_T* parent = nullptr;
		SymbolTable arguments;

	public:
		Template() = default;

		Parent_T* GetParent() const;
		SymbolTable GetArguments() const;
		void SetParent(Parent_T* structure); 
		Symbol* InstallArgument(const ID_T& id, Structure* structure);
	};

	// ----------------------------------------------------------------------------------------

	class Definition : public Symbol {
	private:
		Structure* type = nullptr;	// Structure type for fields, nullptr for in-method definitions.
		std::string full_type = "";	// Definition type.

	protected:
		DEBUG_FRIENDLY virtual bool IsEqual(const Symbol& other) const override;

	public:
		Definition() : Symbol(ClassType::Definition) {};
		Definition(const ID_T& id, const std::string& name, const std::string& nameSpace = "", Structure* type = nullptr)
			: Symbol(id, name, nameSpace, ClassType::Definition), type(type) {};
		Definition(const ID_T& id, const std::string& name, const std::string& nameSpace, Structure* type, const std::string& fileName, int line, int column)
			: Symbol(id, name, nameSpace, ClassType::Definition, fileName, line, column), type(type) {};

		bool isStructure() const;
		const Structure* GetType() const;
		std::string GetFullType() const;
		void SetType(Structure* structure);
		void SetFullType(const std::string& type);

		DEBUG_FRIENDLY virtual void Output(std::ostream& os) const override;

		DEBUG_FRIENDLY bool operator==(const Definition& rhs) const;
	};

	DEBUG_FRIENDLY inline bool Definition::operator==(const Definition& rhs) const {
		return IsEqual(rhs);
	}

	DEBUG_FRIENDLY inline bool operator!=(const Definition& lhs, const Definition& rhs) {
		return !(lhs == rhs);
	}

	DEBUG_FRIENDLY inline std::ostream& operator<<(std::ostream& os, const Definition& d) {
		d.Output(os);
		return os;
	}

	#define Value_mem_t "Value"
	#define ClassField_mem_t "ClassField"
	#define ClassMethod_mem_t "ClassMethod"
	#define MethodDefinition_mem_t "MethodDefinition"

	class Method : public Symbol {
	public:
		class Member {
		public:
		using MemberType = std::string; 
		private:
			std::string name;
			SourceInfo locEnd;
			Structure* type;
			MemberType memType = Value_mem_t;

		public:
			Member() = default;
			Member(const std::string& name, Structure* type, SourceInfo locEnd, MemberType memType = Value_mem_t) : name(name), type(type), locEnd(locEnd), memType(memType) {};
			std::string GetName() const;
			SourceInfo GetLocEnd() const;
			Structure* GetType() const;
			MemberType GetMemberType() const;

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
		std::map<std::string, MemberExpr> memberExprs;	// <location, MemberExpr>
		int literals = 0;
		int statements = 0;
		int branches = 0;
		int loops = 0;
		int max_scope_depth = 0;
		int line_count = 0;
		bool is_virtual;
	
	protected:
		DEBUG_FRIENDLY virtual bool IsEqual(const Symbol& other) const override;

	public:
		Method() : Symbol(ClassType::Method) {};
		Method(const ID_T& id, const std::string& name, const std::string& nameSpace = "") : Symbol(id, name, nameSpace, ClassType::Method) {};
		Method(const ID_T& id, const std::string& name, const std::string& nameSpace, const std::string& fileName, int line, int column)
			: Symbol(id, name, nameSpace, ClassType::Method, fileName, line, column) {};

		MethodType GetMethodType() const;
		std::string GetMethodTypeAsString() const;
		Structure* GetReturnType() const;

		SymbolTable GetArguments() const;
		SymbolTable GetDefinitions() const;
		SymbolTable GetTemplateArguments() const;
		std::map<std::string, MemberExpr> GetMemberExpr() const;
		int GetLiterals() const;
		int GetStatements() const;
		int GetBranches() const;
		int GetLoops() const;
		int GetMaxScopeDepth() const;
		int GetLineCount() const;

		void SetMethodType(const MethodType& type);
		void SetReturnType(Structure* structure);
		void SetTemplateParent(Method* structure);
		void SetLiterals(int literals);
		void SetStatements(int statements);
		void SetBranches(int branches);
		void SetLoops(int loops);
		void SetMaxScopeDepth(int max_scope_depth);
		void SetLineCount(int line_count);
		void SetVirtual(bool is_virtual);

		Symbol* InstallArg(const ID_T& id, const Definition& definition);
		Symbol* InstallDefinition(const ID_T& id, const Definition& definition);
		Symbol* InstallTemplateSpecializationArgument(const ID_T& id, Structure* structure);

		void InsertMemberExpr(MemberExpr const& memberExpr, Member const& member, const std::string& locBegin);
		void UpdateMemberExpr(MemberExpr const& memberExpr, const std::string& locBegin);

		bool IsConstructor () const;
		bool IsDestructor() const;
		bool IsUserMethod() const;
		bool IsOverloadedOperator() const;
		bool IsTemplateDefinition() const;
		bool IsTemplateFullSpecialization() const;
		bool IsTemplateInstantiationSpecialization() const;
		bool IsTrivial() const;
		bool IsVirtual() const;

		DEBUG_FRIENDLY virtual void Output(std::ostream& os) const override;

		DEBUG_FRIENDLY bool operator==(const Method& rhs) const;
	};

	DEBUG_FRIENDLY inline bool Method::operator==(const Method& rhs) const {
		return IsEqual(rhs);
	}

	DEBUG_FRIENDLY inline bool operator!=(const Method& lhs, const Method& rhs) {
		return !(lhs == rhs);
	}

	DEBUG_FRIENDLY inline std::ostream& operator<<(std::ostream& os, const Method& m) {
		m.Output(os);
		return os;
	}

	// ----------------------------------------------------------------------------------------

	class Structure : public Symbol { // TODO Remove name??
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
	
	protected:
		DEBUG_FRIENDLY virtual bool IsEqual(const Symbol& other) const override;

	public:
		Structure() : Symbol(ClassType::Structure) {};
		Structure(const ID_T& id, const std::string& name, const std::string& nameSpace = "", StructureType structureType = StructureType::Undefined)
			: Symbol(id, name, nameSpace, ClassType::Structure), structureType(structureType) {};
		Structure(const ID_T& id, const std::string& name, const std::string& nameSpace, StructureType structureType, const std::string& fileName, int line, int column)
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

		Symbol* LookupMethod(const ID_T& id);

		Symbol* InstallMethod(const ID_T& id, const Method& method);
		Symbol* InstallField(const ID_T& id, const Definition& definition);
		Symbol* InstallBase(const ID_T& id, Structure* structure);
		Symbol* InstallNestedClass(const ID_T& id, Structure* structure);
		Symbol* InstallFriend(const ID_T& id, Structure* structure);
		Symbol* InstallTemplateSpecializationArgument(const ID_T& id, Structure* structure);

		bool IsTemplateDefinition() const;
		bool IsTemplateFullSpecialization() const;
		bool IsTemplateInstantiationSpecialization() const;
		bool IsTemplatePartialSpecialization() const;
		bool IsTemplate() const;
		bool IsUndefined() const;
		bool IsNestedClass() const;

		DEBUG_FRIENDLY virtual void Output(std::ostream& os) const override;

		DEBUG_FRIENDLY bool operator==(const Structure& rhs) const;
	};

	DEBUG_FRIENDLY inline bool Structure::operator==(const Structure& rhs) const {
		return IsEqual(rhs);
	}

	DEBUG_FRIENDLY inline bool operator!=(const Structure& lhs, const Structure& rhs) {
		return !(lhs == rhs);
	}

	DEBUG_FRIENDLY inline std::ostream& operator<<(std::ostream& os, const Structure& s) {
		s.Output(os);
		return os;
	}

	/*class Fundamental : public Symbol {

	};*/

	

}