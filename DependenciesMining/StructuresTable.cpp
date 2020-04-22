#include "StructuresTable.h"

using namespace DependenciesMining;

// SourceInfo 
std::string SourceInfo::GetFileName() const {
	return fileName;
}

int SourceInfo::GetLine() const {
	return line;
}
int SourceInfo::GetColumn() const {
	return column;
}

std::string SourceInfo::toString() const {
	return fileName + ":" + std::to_string(line) + ":" + std::to_string(column);
}

void SourceInfo::SetFileName(std::string name) {
	fileName = name;
}

void SourceInfo::SetLine(int line) {
	this->line = line;
}

void SourceInfo::SetColumn(int column) {
	this->column = column;
}

void SourceInfo::SetTotalLocation(std::string fileName, int line, int column) {
	this->fileName = fileName;
	this->line = line;
	this->column = column;
}

bool  SourceInfo::operator<(const SourceInfo const& loc) const {
	assert(fileName == loc.fileName);
	if (line < loc.line)
		return true;
	else if (column < loc.column)
		return true;
	return false;
}

bool  SourceInfo::operator<=(SourceInfo const& loc) const {
	assert(fileName == loc.fileName);
	if (line <= loc.line)
		return true;
	else if (column <= loc.column)
		return true;
	return false;
}

bool  SourceInfo::operator>(SourceInfo const& loc) const {
	assert(fileName == loc.fileName);
	if (line > loc.line)
		return true;
	else if (column > loc.column)
		return true;
	return false;
}

bool  SourceInfo::operator>=(SourceInfo const& loc) const {
	assert(fileName == loc.fileName);
	if (line >= loc.line)
		return true;
	else if (column >= loc.column)
		return true;
	return false;
}

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

MethodType Method::GetMethodType() const {
	return methodType;
}

Structure* Method::GetReturnType() const {
	return returnType;
}

void Method::SetMethodType(MethodType type) {
	methodType = type;
}

void Method::SetReturnType(Structure* structure) {
	returnType = structure;
}

void Method::InsertArg(const std::string& name, Definition& definition) {
	if (arguments.find(name) == arguments.end())
		arguments[name] = definition;
}

void Method::InsertDefinition(const std::string& name, Definition& definition) {
	if (definitions.find(name) == definitions.end())
		definitions[name] = definition;
}

void Method::InsertMemberExpr(MemberExpr const& memberExpr, Member const& member, const std::string& locBegin) {
	if (memberExprs.find(locBegin) == memberExprs.end()) {
		memberExprs[locBegin] = memberExpr;
	}
	else {
		if (memberExpr.GetLocEnd() > memberExprs[locBegin].GetLocEnd()) {
			memberExprs[locBegin].SetExpr(memberExpr.GetExpr());
			memberExprs[locBegin].SetLocEnd(memberExpr.GetLocEnd());
		}
	}
	memberExprs[locBegin].InsertMember(member);
}

// MemberExpr 
std::string Method::MemberExpr::GetExpr() const {
	return expr;
}
std::vector<Method::Member> Method::MemberExpr::GetMembers() const {
	return members;
}

SourceInfo Method::MemberExpr::GetLocEnd() const {
	return locEnd;
}
void Method::MemberExpr::SetExpr(std::string expr) {
	this->expr = expr;
}

void Method::MemberExpr::SetLocEnd(SourceInfo locEnd) {
	this->locEnd = locEnd;
}

void Method::MemberExpr::InsertMember(Member member) {
	members.push_back(member);
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
Structure* Structure::GetNestedParent() const {
	return nestedParent;
}

Method* Structure::GetMethod(const std::string& name) {
	auto it = methods.find(name);
	if (it != methods.end())
		return  &it->second;
	else
		return nullptr;
}

std::unordered_map<std::string, Method>& Structure::GetMethods() {
	return methods;
}

std::unordered_map<std::string, Definition>& Structure::GetFields() {
	return fields;
}

std::unordered_map<std::string, Structure*>& Structure::GetBases() {
	return bases;
}

std::unordered_map<std::string, Structure*>& Structure::GetContains() {
	return contains;
}

std::unordered_map<std::string, Structure*>& Structure::GetFriends()
{
	return friends;
}

void Template::SetParent(Structure* structure) {
	parent = structure;
}
void Template::InsertSpecializationArguments(const std::string& name, Structure* structure) {
	specializationArguments[name] = structure;
}

void Structure::SetName(const std::string& name) {
	this->name = name;
}

void Structure::SetEnclosingNamespace(const std::string& enclosingNamespace) {
	this->enclosingNamespace = enclosingNamespace;
}

void  Structure::SetStructureType(StructureType structureType) {
	this->structureType = structureType;
}

void Structure::SetTemplateInfo(Template temp)
{
	templateInfo = temp;
}

void Structure::SetTemplateParent(Structure* structure) {
	templateInfo.SetParent(structure);
}

void Structure::SetNestedParent(Structure* structure)
{
	nestedParent = structure;
}

/*
// methods with key retType
Method* Structure::InsertMethod(const std::string& returnTypeName, const std::string& methodName, Method& method) {
	if (methods.find(returnTypeName) == methods.end())
		//if (methods[returnTypeName][methodName] == methods.end())
		methods[returnTypeName][methodName] = method;
	else if (methods[returnTypeName].find(methodName) == methods[returnTypeName].end())
		methods[returnTypeName][methodName] = method;
	return &methods[returnTypeName][methodName];//->second;
}*/

Method* Structure::InsertMethod(const std::string& name, Method& method) {
	if (methods.find(name) == methods.end())
		methods[name] = method;
	return &methods.find(name)->second;
}

void Structure::InsertField(const std::string& name, Definition& definition) {
	fields[name] = definition;
}

void Structure::InsertBase(const std::string& name, Structure* structure) {
	bases[name] = structure;
}

void Structure::InsertNestedClass(const std::string& name, Structure* structure) {
	contains[name] = structure;
}

void Structure::InsertFriend(const std::string& name, Structure* structure)
{
	friends[name] = structure;
}

void Structure::InsertTemplateSpecializationArguments(const std::string& name, Structure* structure) {
	templateInfo.InsertSpecializationArguments(name, structure);
}


bool Structure::IsTemplateDefinition() {
	if (structureType == StructureType::TemplateDefinition)
		return true;
	return false;
}

bool Structure::IsTemplateFullSpecialization() {
	if (structureType == StructureType::TemplateFullSpecialization)
		return true;
	return false;
}

bool Structure::IsTemplateInstatiationSpecialization() {
	if (structureType == StructureType::TemplateInstatiationSpecialization)
		return true;
	return false;
}

bool Structure::IsTemplatePartialSpecialization() {
	if (structureType == StructureType::TemplatePartialSpecialization)
		return true;
	return false;
}

bool Structure::IsTemplate() {
	if (structureType >= StructureType::TemplateDefinition && structureType <= StructureType::TemplatePartialSpecialization)
		return true;
	return false;
}

bool Structure::IsNestedClass() {
	if (nestedParent)
		return true;
	return false;
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

Structure* StructuresTable::Get(const std::string& name) {
	auto it = table.find(name);
	if (it != table.end())
		return &it->second;
	else
		return nullptr;
}

void StructuresTable::Print() {
	for (auto& t : table) {
		std::cout << "Name: " << t.first << std::endl;
		std::cout << "--------------------------------------------\n";
	}
}