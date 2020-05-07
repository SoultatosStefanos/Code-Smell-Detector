#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cassert>
#include <Vector>

#define MEMBER_MAP std::unordered_map <std::string, std::unordered_map<std::string, std::unordered_map<std::string, Method::MemberExpr>>>

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
		std::string name;
		Structure* type;

	public:
		Definition() = default;
		Definition(const std::string& name, Structure* type) : name(name), type(type) {};
		Definition(const std::string& name, Structure* type, std::string fileName, int line, int column) : name(name), type(type), SourceInfo(fileName, line, column) {};
		std::string GetName() const;
		const Structure* GetType() const;
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
		std::string name;
		MethodType methodType = MethodType::Undefined;
		Structure* returnType = nullptr;
		std::unordered_map<std::string, Definition> arguments;
		std::unordered_map<std::string, Definition> definitions;
		std::unordered_map<std::string, MemberExpr> memberExprs;	// <location, MemberExpr>
	public:
		Method() = default;
		Method(std::string name) : name(name) {};
		Method(std::string name, std::string fileName, int line, int column) : name(name), SourceInfo(fileName, line, column) {};
		std::string GetName() const;
		std::unordered_map<std::string, Definition>& GetArguments();
		std::unordered_map<std::string, Definition>& GetDefinitions();
		MEMBER_MAP& GetMemberExprs();
		MethodType GetMethodType() const;
		Structure* GetReturnType() const;

		void SetMethodType(MethodType type);
		void SetReturnType(Structure* structure);
		void InsertArg(const std::string& name, Definition& definition);
		void InsertDefinition(const std::string& name, Definition& definition);
		void InsertMemberExpr(MemberExpr const& memberExpr, Member const& member, const std::string& locBegin);
		void UpdateMemberExpr(MemberExpr const& memberExpr, const std::string& locBegin);

		bool isConstructor(); 
		bool isDestructor();
		bool isUserMethod();
		bool isTemplateDefinition();
		bool isTemplateFullSpecialization();
		bool isTemplateInstatiationSpecialization();
	};

	class Template {
	private:
		Structure* parent = nullptr;
		std::unordered_map<std::string, Structure*> specializationArguments;

	public:
		Template() = default;
		void SetParent(Structure* structure);
		void InsertSpecializationArguments(const std::string& name, Structure* structure);
	};

	class Structure : public SourceInfo {
	private:
		std::string name;
		std::string enclosingNamespace = "";
		StructureType structureType = StructureType::Undefined;
		Template templateInfo;
		Structure* nestedParent = nullptr;
		std::unordered_map<std::string, Method> methods;
		std::unordered_map<std::string, Definition> fields;
		std::unordered_map<std::string, Structure*> bases;
		std::unordered_map<std::string, Structure*> contains;
		std::unordered_map<std::string, Structure*> friends;	// About Structures: Key->structureName, Value->Structure*
																// About Methods: Key->methodName, Value->Structure* (the parent Class which owns this method)

	public:
		Structure(Structure& structure);
		Structure() = default;
		Structure(std::string name, std::string enclosingNamespace = "", StructureType structureType = StructureType::Undefined)
			: name(name), enclosingNamespace(enclosingNamespace), structureType(structureType) {};
		Structure(std::string name, std::string enclosingNamespace, StructureType structureType, std::string fileName, int line, int column)
			: name(name), enclosingNamespace(enclosingNamespace), structureType(structureType), SourceInfo(fileName, line, column) {};
		std::string GetName() const;
		std::string GetEnclosingNamespace() const;
		StructureType GetStructureType() const;
		Structure* GetNestedParent() const;
		Method* GetMethod(const std::string& name);
		std::unordered_map<std::string, Method>& GetMethods();
		std::unordered_map<std::string, Definition>& GetFields();
		std::unordered_map<std::string, Structure*>& GetBases();
		std::unordered_map<std::string, Structure*>& GetContains();
		std::unordered_map<std::string, Structure*>& GetFriends();

		void SetName(const std::string& name);
		void SetEnclosingNamespace(const std::string& enclosingNamespace);
		void SetStructureType(StructureType structureType);
		void SetTemplateInfo(Template temp);
		void SetTemplateParent(Structure* structure);
		void SetNestedParent(Structure* structure);

		//Method* InsertMethod(const std::string& returnTypeName, const std::string& methodName, Method& method);
		Method* InsertMethod(const std::string& name, Method& method);
		void InsertField(const std::string& name, Definition& definition);
		void InsertBase(const std::string& name, Structure* structure);
		void InsertNestedClass(const std::string& name, Structure* structure);
		void InsertFriend(const std::string& name, Structure* structure);
		void InsertTemplateSpecializationArguments(const std::string& name, Structure* structure);

		bool IsTemplateDefinition();
		bool IsTemplateFullSpecialization();
		bool IsTemplateInstatiationSpecialization();
		bool IsTemplatePartialSpecialization();
		bool IsTemplate();
		bool IsNestedClass();
	};


	class StructuresTable {
	private:
		std::unordered_map<std::string, Structure> table;
	public:
		std::unordered_map<std::string, Structure>& GetSymbolTable();
		Structure* Insert(const std::string& name);
		Structure* Insert(const std::string& name, Structure& structure);
		Structure* Get(const std::string& name);

		void Print();
	};

	// Returns the number of dependencies that Structure "from" has on Structure "to"
	int GetIntencity(Structure* from, Structure* to);
	//int GetIntencity(std::string* from, std::string* to);
}