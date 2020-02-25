#include "StructuresTable.h"

using namespace DependenciesMining;

//Definition
std::string Definition::GetName() const {
	return name;
}

const Structure* Definition::GetType() const {
	return type;
}


// Method
std::string Method::GetName() const {
	return name;
}

std::unordered_map<std::string, Definition>& Method::GetArguments() {
	return arguments;
}

std::unordered_map<std::string, Definition>& Method::GetDefinitions() {
	return definitions;
}

void Method::InsertArg(const std::string& name, Definition& definition) {
	if (arguments.find(name) == arguments.end())
		arguments[name] = definition;
}

void Method::InsertDefinition(const std::string& name, Definition& definition) {
	if (definitions.find(name) == definitions.end())
		definitions[name] = definition;
}


// Structure
Structure::Structure(Structure& structure) {
	std::cout << "Structure copy const\n";
}

std::string Structure::GetName() const {
	return name;
}

std::string Structure::GetEnclosingNamespace() const { 
	return enclosingNamespace; 
}

StructureType Structure::GetStructureType() const { 
	return structureType; 
}

std::unordered_map<std::string, Method>& Structure::GetMethods() { 
	return methods; 
}

std::unordered_map<std::string, Definition>& Structure::GetFields() { 
	return fields; 
}

std::unordered_map<std::string, Structure*>& Structure::GetBases()  { 
	return bases; 
}

std::unordered_map<std::string, Structure*>& Structure::GetFriends()
{
	return friends;
}

void Structure::SetName(const std::string& name) {
	this->name = name;
}

void Structure::SetEnclosingNamespace(const std::string& enclosingNamespace) {
	this->enclosingNamespace = enclosingNamespace;
}

void  Structure::SetStructureType(StructureType structureType){
	this->structureType = structureType;
}


void Structure::InsertMethod(const std::string& name, Method& method) {
	if (methods.find(name) == methods.end())
		methods[name] = method;
}

void Structure::InsertField(const std::string& name, Definition& definition) {
		fields[name] = definition;
}

void Structure::InsertBase(const std::string& name, Structure* structure) {
		bases[name] = structure;
}

void Structure::InsertFriend(const std::string& name, Structure* structure)
{
	friends[name] = structure;
}

// StructuresTable
std::unordered_map<std::string, Structure>& StructuresTable::GetSymbolTable() {
	return table; 
}

Structure* StructuresTable::Insert(const std::string& name) {
	auto it = table.find(name);
	if (it != table.end()) 
		return &it->second;

	Structure dummy(name);
	table[name] = dummy;
	return &table.find(name)->second;
}

Structure* StructuresTable::Insert(const std::string& name, Structure& structure) {
	auto it = table.find(name);
	if (it != table.end() && it->second.GetStructureType() != StructureType::Undefined)
		return &it->second;

	table[name] = structure; 
	return &table.find(name)->second; 
}

Structure* StructuresTable::Get(std::string& name) {
	auto it = table.find(name);
	if (it != table.end())
		return &it->second;
	else
		return nullptr;
}

void StructuresTable::Print() {
	for (auto &t : table) {
		std::cout << "Name: " << t.first << std::endl;
		std::cout << "--------------------------------------------\n";
	}
}




