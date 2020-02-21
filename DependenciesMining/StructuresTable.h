#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cassert>

namespace DependenciesMining {

	class Structure;

	enum class StructureType {
		Undefined,
		Class,
		Struct,
		TemplateSpecialization
	};


	class Definition {
	private:
		std::string name;
		Structure* type;

	public:
		Definition() = default;
		Definition(const std::string& name, Structure* type) : name(name), type(type) {};
		std::string GetName() const;
		const Structure* GetType() const;
	};


	class Method {
	private:
		std::string name;
		std::unordered_map<std::string, Definition> arguments;
		std::unordered_map<std::string, Definition> definitions;

	public:
		std::string GetName() const;
		std::unordered_map<std::string, Definition>& GetArguments() ;
		std::unordered_map<std::string, Definition>& GetDefinitions();

		void InsertArg(const std::string& name, Definition& definition);
		void InsertDefinition(const std::string& name, Definition& definition);
	};


	class Structure {
	private:
		std::string name;
		std::string enclosingNamespace = "";
		StructureType structureType = StructureType::Undefined;
		Structure* contained = nullptr;
		std::unordered_map<std::string, Method> methods;
		std::unordered_map<std::string, Definition> fields;
		std::unordered_map<std::string, Structure*> bases;
		
	public:
		Structure(Structure& structure);
		Structure() = default;
		Structure(std::string name, std::string enclosingNamespace = "", StructureType structureType = StructureType::Undefined)
			: name(name), enclosingNamespace(enclosingNamespace), structureType(structureType) {};
		std::string GetName() const;
		std::string GetEnclosingNamespace() const;
		StructureType GetStructureType() const;
		std::unordered_map<std::string, Method>& GetMethods();
		std::unordered_map<std::string, Definition>& GetFields();
		std::unordered_map<std::string, Structure*>& GetBases();
		
		void SetName(const std::string& name);
		void SetEnclosingNamespace(const std::string& enclosingNamespace);
		void SetStructureType(StructureType structureType);

		void InsertMethod(const std::string& name, Method& method);
		void InsertField(const std::string& name, Definition& definition);
		void InsertBase(const std::string& name, Structure* structure);
	};


	class StructuresTable {
	private:
		std::unordered_map<std::string, Structure> table;
	public: 
		std::unordered_map<std::string, Structure>& GetSymbolTable(); 
		Structure* Insert(const std::string& name);
		Structure* Insert(const std::string& name, Structure& structure);
		Structure* Get(std::string& name);

		void Print();
	};
}