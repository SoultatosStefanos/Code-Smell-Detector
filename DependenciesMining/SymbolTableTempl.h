#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cassert>
#include <Vector>
#include <type_traits>

#define ID_T int64_t 

namespace DependenciesMining {

	template<typename T> class SymbolTable;
	class Structure;
	class Definition; 
	class Method;

	enum class StructureType {
		Undefined,
		Class,
		Struct,
		TemplateDefinition,
		TemplateFullSpecialization,
		TemplateInstatiationSpecialization,
		TemplatePartialSpecialization
	};


	enum class MethodType {
		Undefined,
		Constructor,
		Destructor,
		UserMethod,
		TemplateDefinition,
		TemplateFullSpecialization,
		TemplateInstatiationSpecialization
	};


	enum class ClassType {
		Undefined,
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
		virtual const SourceInfo& GetSourceInfo() const;
		virtual std::string GetNamespace() const;

		virtual void SetID(const ID_T id);
		virtual void SetName(const std::string& name);
		virtual void SetClassType(const ClassType& type);
		virtual void SetSourceInfo(const SourceInfo& info);
		virtual void SetSourceInfo(const std::string& fileName, int line, int column);
		virtual void SetNamespace(const std::string& nameSpace);
	};

	template<typename T> class SymbolTable {
	//	static_assert(std::is_base_of<Symbol, T>::value);
	private:
		std::unordered_map<ID_T, T*> byID;
		std::unordered_map<std::string, std::list<T*>> byName;
	public:
		Symbol* Install(ID_T id, const std::string& name);
		Symbol* Install(ID_T id, const T& symbol);
		Symbol* Install(ID_T id, T* symbol);
		Symbol* Lookup(ID_T id);
		Symbol* Lookup(const std::string& name);
		const Symbol* Lookup(ID_T id) const;
		const Symbol* Lookup(const std::string& name) const;

		void Print();
	};


	template<typename Parent_T> class Template {
	private:
		Parent_T* parent = nullptr;
		SymbolTable<Structure> arguments;

	public:
		Template() = default;

		Parent_T* GetParent() const;
		void SetParent(Parent_T* structure); 
		Symbol* InstallArguments(ID_T id, Structure* structure);
	};


	class Definition : public Symbol {
	private:
		Structure* type = nullptr;

	public:
		Definition() : Symbol(ClassType::Definition) {};
		Definition(ID_T id, const std::string& name, const std::string& nameSpace, Structure* type)
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
			void SetExpr(std::string expr);
			void SetLocEnd(SourceInfo locEnd);
			void InsertMember(Member member);
		};

	private:
		MethodType methodType = MethodType::Undefined;
		Structure* returnType = nullptr;
		Template<Method> templateInfo;								// Template Parent is *not* used
		SymbolTable<Symbol> arguments;
		SymbolTable<Symbol> definitions;
		std::unordered_map<std::string, MemberExpr> memberExprs;	// <location, MemberExpr>
	public:
		Method() : Symbol(ClassType::Method) {};
		Method(ID_T id, const std::string& name, const std::string& nameSpace) : Symbol(id, name, nameSpace, ClassType::Method) {};
		Method(ID_T id, const std::string& name, const std::string& nameSpace, const std::string& fileName, int line, int column) 
			: Symbol(id, name, nameSpace, ClassType::Method, fileName, line, column) {};

		MethodType GetMethodType() const;
		Structure* GetReturnType() const;

		void SetMethodType(const MethodType& type);
		void SetReturnType(Structure* structure);
		void SetTemplateParent(Method* structure);

		void InstallArg(ID_T id, const Definition& definition);
		void InstallDefinition(ID_T id, const Definition& definition);
		void InstallTemplateSpecializationArguments(ID_T id, Structure* structure);

		void InsertMemberExpr(MemberExpr const& memberExpr, Member const& member, const std::string& locBegin);
		void UpdateMemberExpr(MemberExpr const& memberExpr, const std::string& locBegin);

		bool isConstructor(); 
		bool isDestructor();
		bool isUserMethod();
		bool isTemplateDefinition();
		bool isTemplateFullSpecialization();
		bool isTemplateInstatiationSpecialization();
	};


	class Structure : public Symbol {
	private:
		
		StructureType structureType = StructureType::Undefined;
		Template<Structure> templateInfo;
		Structure* nestedParent = nullptr;
		SymbolTable<Symbol> methods;
		SymbolTable<Symbol> fields;
		SymbolTable<Symbol> bases;
		SymbolTable<Symbol> contains;
		SymbolTable<Symbol> friends;	// About Structures: Key->structureID, Value->Structure*
								// About Methods: Key->methodID, Value->Structure* (the parent Class which owns this method)
	public:
		Structure() : Symbol(ClassType::Structure) {};
		Structure(ID_T id, const std::string& name, const std::string& nameSpace = "", StructureType structureType = StructureType::Undefined)
			: Symbol(id, name, nameSpace, ClassType::Structure), structureType(structureType) {};
		Structure(ID_T id, const std::string& name, const std::string& nameSpace, StructureType structureType, const std::string& fileName, int line, int column)
			: Symbol(id, name, nameSpace, ClassType::Structure, fileName, line, column), structureType(structureType) {};
		
		StructureType GetStructureType() const;
		Structure* GetTemplateParent() const;
		Structure* GetNestedParent() const;

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

		bool IsTemplateDefinition();
		bool IsTemplateFullSpecialization();
		bool IsTemplateInstatiationSpecialization();
		bool IsTemplatePartialSpecialization();
		bool IsTemplate();
		bool IsNestedClass();
	};


	template<typename T> Symbol* SymbolTable<T>::Install(ID_T id, const std::string& name) {
		auto it = byID.find(id);
		if (it != byID.end())
			return it->second;

		//Symbol* dummy = new Symbol(id, name, "", type);
		T* dummy = new T(id, name, "");
		byID[id] = dummy;
		auto& nameList = byName[name];
		nameList.push_back(dummy);

		return dummy;
	}

	template<typename T> Symbol* SymbolTable<T>::Install(ID_T id, const T& symbol) {
		auto it = byID.find(id);
		if (it != byID.end()) {
			if (symbol.GetClassType() == ClassType::Structure && ((Structure*)(it->second))->GetStructureType() == StructureType::Undefined) {
				*(it->second) = symbol;
			}
			return it->second;
		}

		T* newSymbol = new T(symbol);
		byID[id] = newSymbol;
		auto& nameList = byName[symbol.GetName()];
		nameList.push_back(newSymbol);
		return newSymbol;
	}

	template<typename T> Symbol* SymbolTable<T>::Install(ID_T id, T* symbol) {
		auto it = byID.find(id);
		if (it != byID.end()) {
			if (symbol->GetClassType() == ClassType::Structure && ((Structure*)(it->second))->GetStructureType() == StructureType::Undefined) {
				it->second = symbol;
			}
			return it->second;
		}

		byID[id] = symbol;
		auto& nameList = byName[symbol->GetName()];
		nameList.push_back(symbol);

		return symbol;
	}

	template<typename T> Symbol* SymbolTable<T>::Lookup(ID_T id) {
		auto it = byID.find(id);
		if (it != byID.end())
			return it->second;
		else
			return nullptr;
	}


	template<typename T> Symbol* SymbolTable<T>::Lookup(const std::string& name) {
		auto it = byName.find(name);
		if (it != byName.end()) {
			//assert(it->second.size() == 1);
			return it->second.front();
		}
		else
			return nullptr;
	}

	template<typename T> const Symbol* SymbolTable<T>::Lookup(ID_T id) const {
		auto it = byID.find(id);
		if (it != byID.end())
			return it->second;
		else
			return nullptr;
	}


	template<typename T> const Symbol* SymbolTable<T>::Lookup(const std::string& name) const {
		auto it = byName.find(name);
		if (it != byName.end()) {
			//assert(it->second.size() == 1);
			return it->second.front();
		}
		else
			return nullptr;
	}

	template<typename T> void SymbolTable<T>::Print() {
		for (auto& t : byName) {
			std::cout << "Name: " << t.first << std::endl;
			std::cout << "--------------------------------------------\n";
		}
	}
}