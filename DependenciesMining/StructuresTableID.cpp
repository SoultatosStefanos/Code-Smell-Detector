#include "StructuresTableID.h"

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

ID_T Definition::GetId(ID_T id) const {
	return id;
}

std::string Definition::GetName() const {
	return name;
}

const Structure* Definition::GetType() const {
	return type;
}


// Method
ID_T Method::GetID(ID_T id) const {
	return id;
}


std::string Method::GetName() const {
	return name;
}

std::unordered_map<ID_T, Definition>& Method::GetArguments() {
	return arguments;
}

std::unordered_map<ID_T, Definition>& Method::GetDefinitions() {
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

void Method::SetTemplateInfo(Template<Method> temp)
{
	templateInfo = temp;
}

void Method::SetTemplateParent(Method* method) {
	templateInfo.SetParent(method);
}

void Method::InsertArg(ID_T id, Definition& definition) {
	if (arguments.find(id) == arguments.end())
		arguments[id] = definition;
}

void Method::InsertDefinition(ID_T id, Definition& definition) {
	if (definitions.find(id) == definitions.end())
		definitions[id] = definition;
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

void Method::UpdateMemberExpr(MemberExpr const& memberExpr, const std::string& locBegin) {
	if (memberExprs.find(locBegin) == memberExprs.end()) {
		memberExprs[locBegin] = memberExpr;
	}
	else {
		if (memberExpr.GetLocEnd() > memberExprs[locBegin].GetLocEnd()) {
			memberExprs[locBegin].SetExpr(memberExpr.GetExpr());
			memberExprs[locBegin].SetLocEnd(memberExpr.GetLocEnd());
		}
	}
}

void Method::InsertTemplateSpecializationArguments(ID_T id, Structure* structure) {
	templateInfo.InsertSpecializationArguments(id, structure);
}

bool Method::isConstructor() {
	if (methodType == MethodType::Constructor)
		return true; 
	return false;
}

bool Method::isDestructor() {
	if (methodType == MethodType::Destructor)
		return true;
	return false;
}

bool Method::isUserMethod() {
	if (methodType == MethodType::UserMethod)
		return true;
	return false;
}

bool Method::isTemplateDefinition() {
	if (methodType == MethodType::TemplateDefinition)
		return true;
	return false;
}

bool Method::isTemplateFullSpecialization() {
	if (methodType == MethodType::TemplateFullSpecialization)
		return true;
	return false;
}

bool Method::isTemplateInstatiationSpecialization() {
	if (methodType == MethodType::TemplateInstatiationSpecialization)
		return true;
	return false;
}


// Member
std::string Method::Member::GetName() const {
	return name;
}

SourceInfo Method::Member::GetLocEnd() const {
	return locEnd;
}

Structure* Method::Member::GetType() const {
	return type;
}

void Method::Member::SetName(const std::string& name) {
	this->name = name; 
}

void Method::Member::SetLocEnd(const SourceInfo& locEnd) {
	this->locEnd = locEnd; 
}

void Method::Member::SetType(Structure* type) {
	this->type = type;
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
}

ID_T Structure::GetID() const {
	return id;
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

Structure* Structure::GetTemplateParent() const {
	return templateInfo.GetParent();
}

Structure* Structure::GetNestedParent() const {
	return nestedParent;
}

Method* Structure::GetMethod(ID_T id) {
	auto it = methods.find(id);
	if (it != methods.end())
		return  &it->second;
	else
		return nullptr;
}

std::unordered_map<ID_T, Method>& Structure::GetMethods() {
	return methods;
}

std::unordered_map<ID_T, Definition>& Structure::GetFields() {
	return fields;
}

std::unordered_map<ID_T, Structure*>& Structure::GetBases() {
	return bases;
}

std::unordered_map<ID_T, Structure*>& Structure::GetContains() {
	return contains;
}

std::unordered_map<ID_T, Structure*>& Structure::GetFriends()
{
	return friends;
}

template<typename Parent_T> Parent_T* Template<Parent_T>::GetParent() const {
	return parent;
}

template<typename Parent_T> void Template<Parent_T>::SetParent(Parent_T* parent){
	this->parent = parent;
}

template<typename Parent_T> void Template<Parent_T>::InsertSpecializationArguments(ID_T id, Structure* structure) {
	specializationArguments[id] = structure;
}

void Structure::SetID(const ID_T id) {
	this->id = id;
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

void Structure::SetTemplateInfo(Template<Structure> temp)
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

Method* Structure::InsertMethod(ID_T id, Method& method) {
	if (methods.find(id) == methods.end())
		methods[id] = method;
	return &methods.find(id)->second;
}

void Structure::InsertField(ID_T id, Definition& definition) {
	fields[id] = definition;
}

void Structure::InsertBase(ID_T id, Structure* structure) {
	bases[id] = structure;
}

void Structure::InsertNestedClass(ID_T id, Structure* structure) {
	contains[id] = structure;
}

void Structure::InsertFriend(ID_T id, Structure* structure)
{
	friends[id] = structure;
}

void Structure::InsertTemplateSpecializationArguments(ID_T id, Structure* structure) {
	templateInfo.InsertSpecializationArguments(id, structure);
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
std::unordered_map<ID_T, Structure>& StructuresTable::GetSymbolTable() {
	return table;
}

Structure* StructuresTable::Insert(ID_T id, const std::string& name) {
	auto it = table.find(id);
	if (it != table.end())
		return &it->second;

	Structure dummy(id, name);
	table[id] = dummy;

	auto& nameList = QualifiedNameTable[name];
	if (std::find(nameList.begin(), nameList.end(), &table.find(id)->second) == nameList.end()) {
		QualifiedNameTable[name].push_back(&table.find(id)->second);
		//nameList.push_back(&table.find(id)->second);
	}
	return &table.find(id)->second;
}

Structure* StructuresTable::Insert(ID_T id, Structure& structure) {
	auto it = table.find(id);
	if (it != table.end() && it->second.GetStructureType() != StructureType::Undefined)
		return &it->second;

	//if (auto s = Get(structure.GetName())) {
	//	if (id != s->GetID() && s->GetStructureType() == StructureType::Undefined) {
	//		auto undefStructure = table.extract(s->GetID());
	//		undefStructure.key() = id; 
	//		table.insert(std::move(undefStructure)); 
	//	}
	//}

	table[id] = structure;

	auto& nameList = QualifiedNameTable[structure.GetName()];
	if (std::find(nameList.begin(), nameList.end(), &table.find(id)->second) == nameList.end()) {
		QualifiedNameTable[structure.GetName()].push_back(&table.find(id)->second);
		//nameList.push_back(&table.find(id)->second);
	}
	return &table.find(id)->second;
}

Structure* StructuresTable::Get(ID_T id) {
	auto it = table.find(id);
	if (it != table.end())
		return &it->second;
	else
		return nullptr;
}


Structure* StructuresTable::Get(const std::string& structureName) {
	if (QualifiedNameTable.find(structureName) != QualifiedNameTable.end()) {
		auto& nameList = QualifiedNameTable[structureName];
		//assert(nameList.size() == 1);
		return nameList.front();			
	}
	return nullptr;
}

void StructuresTable::Print() {
	for (auto& t : table) {
		std::cout << "Name: " << t.first << std::endl;
		std::cout << "--------------------------------------------\n";
	}
}