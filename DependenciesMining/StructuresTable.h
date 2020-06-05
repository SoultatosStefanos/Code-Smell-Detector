#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cassert>
#include <Vector>

#define ID_T int64_t 

namespace DependenciesMining {

	class Structure;

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
		std::string fileName;
		int line;
		int column;
	public:
		SourceInfo() = default;
		SourceInfo(std::string fileName, int line, int column) : fileName(fileName), line(line), column(column) {};
		std::string GetFileName() const;
		int GetLine() const;
		int GetColumn() const;
		std::string toString() const;
		void SetFileName(std::string name);
		void SetLine(int line);
		void SetColumn(int column);
		void SetTotalLocation(const std::string& fileName, int line, int column);

		bool  operator<(SourceInfo const& loc) const;
		bool  operator<=(SourceInfo const& loc) const;
		bool  operator>(SourceInfo const& loc) const;
		bool  operator>=(SourceInfo const& loc) const;
	};

	class Symbol {
		ID_T id;
		std::string qualifiedName;
		std::string enclosingNamespace = "";
		SourceInfo srcInfo;
		ClassType classType;

	public:
		Symbol() = default; 
		Symbol(ClassType classType) : classType(classType) {};
		Symbol(ID_T id, const std::string& qualifiedName, const std::string& enclosingNamespace = "", ClassType classType = ClassType::Undefined, const std::string& fileName = "", int line = -1, int column = -1) : id(id), qualifiedName(qualifiedName), classType(classType), srcInfo(SourceInfo(fileName, line, column)) {};

		ID_T GetID() const;
		std::string GetQualifiedName() const;
		ClassType GetClassType() const; 
		const SourceInfo& GetSourceInfo() const;
		std::string GetEnclosingNamespace() const;

		void SetID(const ID_T id); 
		void SetQualifiedName(const std::string& qualifiedName);
		void SetClassType(ClassType type);
		void SetSourceInfo(const SourceInfo& info);
		void SetSourceInfo(const std::string& fileName, int line, int column);
		void SetEnclosingNamespace(const std::string& enclosingNamespace);
	};




	class Definition : public Symbol {
	private:
		Structure* type;

	public:
		Definition() : Symbol(ClassType::Definition) {};
		Definition(ID_T id, const std::string& qualifiedName, const std::string& enclosingNamespace, Structure* type) : Symbol( id, qualifiedName, enclosingNamespace, ClassType::Definition) , type(type) {};
		Definition(ID_T id, const std::string& qualifiedName, const std::string& enclosingNamespace, Structure* type, const std::string& fileName, int line, int column) : Symbol(id, qualifiedName, enclosingNamespace, ClassType::Definition, fileName, line, column), type(type) {};
		
		const Structure* GetType() const;
		void SetType(Structure* structure);

	};


	template<typename Parent_T> class Template {
	private:
		Parent_T* parent = nullptr;
		std::unordered_map<ID_T, Structure*> specializationArguments;

	public:
		Template() = default;

		Parent_T* GetParent() const;
		void SetParent(Parent_T* structure); 
		void InsertSpecializationArguments(ID_T id, Structure* structure);
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

		class MemberExpr : public SourceInfo {					// SourceInfo: expr position on src code		
			std::string expr;
			SourceInfo locEnd;									// to store the longer expr
			std::vector<Member> members;
		public:
			MemberExpr() = default;
			MemberExpr(std::string expr, SourceInfo locEnd, std::string file, int line, int column) : expr(expr), locEnd(locEnd), SourceInfo(file, line, column) {};
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
		std::unordered_map<ID_T, Definition> arguments;
		std::unordered_map<ID_T, Definition> definitions;
		std::unordered_map<std::string, MemberExpr> memberExprs;	// <location, MemberExpr>
	public:
		Method() : Symbol(ClassType::Method) {};
		Method(ID_T id, const std::string& qualifiedName, const std::string& enclosingNamespace) : Symbol(id, qualifiedName, enclosingNamespace, ClassType::Method) {};
		Method(ID_T id, const std::string& qualifiedName, const std::string& enclosingNamespace, const std::string& fileName, int line, int column) : Symbol(id, qualifiedName, enclosingNamespace, ClassType::Method, fileName, line, column) {};

		std::unordered_map<ID_T, Definition>& GetArguments();
		std::unordered_map<ID_T, Definition>& GetDefinitions();
		MethodType GetMethodType() const;
		Structure* GetReturnType() const;

		void SetMethodType(MethodType type);
		void SetReturnType(Structure* structure);
		void SetTemplateInfo(Template<Method> temp);
		void SetTemplateParent(Method* structure);

		void InsertArg(ID_T id, Definition& definition);
		void InsertDefinition(ID_T id, Definition& definition);
		void InsertMemberExpr(MemberExpr const& memberExpr, Member const& member, const std::string& locBegin);
		void UpdateMemberExpr(MemberExpr const& memberExpr, const std::string& locBegin);
		void InsertTemplateSpecializationArguments(ID_T id, Structure* structure);

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
		std::unordered_map<ID_T, Method> methods;
		std::unordered_map<ID_T, Definition> fields;
		std::unordered_map<ID_T, Structure*> bases;
		std::unordered_map<ID_T, Structure*> contains;
		std::unordered_map<ID_T, Structure*> friends;	// About Structures: Key->structureID, Value->Structure*
														// About Methods: Key->methodID, Value->Structure* (the parent Class which owns this method)

	public:
		Structure() : Symbol(ClassType::Structure) {};
		Structure(ID_T id, const std::string& qualifiedName, const std::string& enclosingNamespace = "", StructureType structureType = StructureType::Undefined)
			: Symbol(id, qualifiedName, enclosingNamespace, ClassType::Structure), structureType(structureType) {};
		Structure(ID_T id, const std::string& qualifiedName, const std::string& enclosingNamespace, StructureType structureType, std::string fileName, int line, int column)
			: Symbol(id, qualifiedName, enclosingNamespace, ClassType::Structure, fileName, line, column), structureType(structureType) {};
		
		StructureType GetStructureType() const;
		Structure* GetTemplateParent() const;
		Structure* GetNestedParent() const;
		Method* GetMethod(ID_T id);
		std::unordered_map<ID_T, Method>& GetMethods();
		std::unordered_map<ID_T, Definition>& GetFields();
		std::unordered_map<ID_T, Structure*>& GetBases();
		std::unordered_map<ID_T, Structure*>& GetContains();
		std::unordered_map<ID_T, Structure*>& GetFriends();

		void SetStructureType(StructureType structureType);
		void SetTemplateInfo(Template<Structure> temp);
		void SetTemplateParent(Structure* structure);
		void SetNestedParent(Structure* structure);

		//Method* InsertMethod(const std::string& returnTypeName, const std::string& methodName, Method& method);
		Method* InsertMethod(ID_T id, Method& method);
		void InsertField(ID_T id, Definition& definition);
		void InsertBase(ID_T id, Structure* structure);
		void InsertNestedClass(ID_T id, Structure* structure);
		void InsertFriend(ID_T id, Structure* structure);
		void InsertTemplateSpecializationArguments(ID_T id, Structure* structure);

		bool IsTemplateDefinition();
		bool IsTemplateFullSpecialization();
		bool IsTemplateInstatiationSpecialization();
		bool IsTemplatePartialSpecialization();
		bool IsTemplate();
		bool IsNestedClass();
	};


	class StructuresTable {
	private:
		std::unordered_map<ID_T, Structure> table;
		std::unordered_map<std::string, std::list<Structure*>> QualifiedNameTable;
	public:
		std::unordered_map<ID_T, Structure>& GetSymbolTable();
		Structure* Insert(ID_T id, const std::string& name);
		Structure* Insert(ID_T id, Structure& structure);
		Structure* Get(ID_T id);
		Structure* Get(const std::string& structureName);

		void Print();
	};

	// Returns the number of dependencies that Structure "from" has on Structure "to"
	int GetIntencity(Structure* from, Structure* to);
	//int GetIntencity(std::string* from, std::string* to);
}