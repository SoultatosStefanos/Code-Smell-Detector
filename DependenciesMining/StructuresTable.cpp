#include "StructuresTable.h"

using namespace DependenciesMining;

// SourceInfo 
std::string SourceInfo::GetFullLocation() const {
	return fullLocation;
}
void SourceInfo::SetFullLocation(std::string fullLocation) {
	this->fullLocation = fullLocation;
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

MEMBER_MAP& Method::GetMemberExprs() {
	return memberExprs;
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

/*
// Version with the Types' Map
void Method::InsertMemberExpr(const std::string& name, Structure* structure, Method::MemberExpr memberExpr) {
	if (memberExprs.find(name) == memberExprs.end()) {
		std::vector<Method::MemberExpr> exprs;
		exprs.push_back(memberExpr);
		memberExprs[name] = std::pair(structure, exprs);
	} 
	else{
		memberExprs[name].second.push_back(memberExpr);
	}
}*/

void Method::InsertMemberExpr(const std::string& locBegin, const std::string& locEnd, const std::string& expr, Method::MemberExpr memberExpr) {
	memberExprs[locBegin][expr][locEnd] = memberExpr;
}

// MemberExpr 
/*
// Version with the Types' Map 
std::string Method::MemberExpr::GetMemberName() const {
	return memberName;
}
std::string  Method::MemberExpr::GetExpr() const {
	return expr;
}

bool  Method::MemberExpr::isMethod() {
	return isMethod_;
}
void  Method::MemberExpr::SetAsMethod(bool b) {
	isMethod_ = b;
}
*/
std::string Method::MemberExpr::GetName() const {
	return name;
}
Structure* Method::MemberExpr::GetType() const {
	return type;
}

bool  Method::MemberExpr::IsMethod() {
	return isMethod;
}
void  Method::MemberExpr::SetAsMethod(bool b) {
	isMethod = b;
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

std::unordered_map<std::string, Structure*>& Structure::GetBases()  { 
	return bases; 
}

std::unordered_map<std::string, Structure*>& Structure::GetContains()  { 
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

void  Structure::SetStructureType(StructureType structureType){
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
	if (structureType >= StructureType::TemplateDefinition  && structureType <= StructureType::TemplatePartialSpecialization)
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
	for (auto &t : table) {
		std::cout << "Name: " << t.first << std::endl;
		std::cout << "--------------------------------------------\n";
	}
}