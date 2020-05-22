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
		void SetTotalLocation(std::string fileName, int line, int column);

		bool  operator<(SourceInfo const& loc) const;
		bool  operator<=(SourceInfo const& loc) const;
		bool  operator>(SourceInfo const& loc) const;
		bool  operator>=(SourceInfo const& loc) const;
	};


	class Definition : public SourceInfo {
	private:
		ID_T id;
		std::string name;
		Structure* type;

	public:
		Definition() = default;
		Definition(ID_T id, const std::string& name, Structure* type) : id(id), name(name), type(type) {};
		Definition(ID_T id, const std::string& name, Structure* type, std::string fileName, int line, int column) : id(id), name(name), type(type), SourceInfo(fileName, line, column) {};
		
		ID_T GetId(ID_T id) const;
		std::string GetName() const;
		const Structure* GetType() const;
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


	class Method : public SourceInfo {
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
		ID_T id;
		std::string name;
		MethodType methodType = MethodType::Undefined;
		Structure* returnType = nullptr;
		Template<Method> templateInfo;
		std::unordered_map<ID_T, Definition> arguments;
		std::unordered_map<ID_T, Definition> definitions;
		std::unordered_map<std::string, MemberExpr> memberExprs;	// <location, MemberExpr>
	public:
		Method() = default;
		Method(ID_T id, std::string name) : id(id), name(name) {};
		Method(ID_T id, std::string name, std::string fileName, int line, int column) : id(id), name(name), SourceInfo(fileName, line, column) {};
		ID_T GetID(ID_T id) const;
		std::string GetName() const;
		std::unordered_map<ID_T, Definition>& GetArguments();
		std::unordered_map<ID_T, Definition>& GetDefinitions();
		MethodType GetMethodType() const;
		Structure* GetReturnType() const;

		void SetID(ID_T id);
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


	class Structure : public SourceInfo {
	private:
		ID_T id; 
		std::string name;
		std::string enclosingNamespace = "";
		StructureType structureType = StructureType::Undefined;
		Template<Structure> templateInfo;
		Structure* nestedParent = nullptr;
		std::unordered_map<ID_T, Method> methods;
		std::unordered_map<ID_T, Definition> fields;
		std::unordered_map<ID_T, Structure*> bases;
		std::unordered_map<ID_T, Structure*> contains;
		std::unordered_map<ID_T, Structure*> friends;	// About Structures: Key->structureName, Value->Structure*
																// About Methods: Key->methodName, Value->Structure* (the parent Class which owns this method)

	public:
		Structure(Structure& structure);
		Structure() = default;
		Structure(ID_T id, std::string name, std::string enclosingNamespace = "", StructureType structureType = StructureType::Undefined)
			: id(id), name(name), enclosingNamespace(enclosingNamespace), structureType(structureType) {};
		Structure(ID_T id, std::string name, std::string enclosingNamespace, StructureType structureType, std::string fileName, int line, int column)
			: id(id), name(name), enclosingNamespace(enclosingNamespace), structureType(structureType), SourceInfo(fileName, line, column) {};
		
		ID_T GetID() const;
		std::string GetName() const;
		std::string GetEnclosingNamespace() const;
		StructureType GetStructureType() const;
		Structure* GetTemplateParent() const;
		Structure* GetNestedParent() const;
		Method* GetMethod(ID_T id);
		std::unordered_map<ID_T, Method>& GetMethods();
		std::unordered_map<ID_T, Definition>& GetFields();
		std::unordered_map<ID_T, Structure*>& GetBases();
		std::unordered_map<ID_T, Structure*>& GetContains();
		std::unordered_map<ID_T, Structure*>& GetFriends();

		void SetID(const ID_T id);
		void SetName(const std::string& name);
		void SetEnclosingNamespace(const std::string& enclosingNamespace);
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
	public:
		std::unordered_map<ID_T, Structure>& GetSymbolTable();
		Structure* Insert(ID_T id, const std::string& name = "");
		Structure* Insert(ID_T id, Structure& structure);
		Structure* Get(ID_T id);

		void Print();
	};

	// Returns the number of dependencies that Structure "from" has on Structure "to"
	int GetIntencity(Structure* from, Structure* to);
	//int GetIntencity(std::string* from, std::string* to);
}