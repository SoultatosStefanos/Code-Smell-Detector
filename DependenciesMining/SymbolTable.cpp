#include "SymbolTable.h"

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

void SourceInfo::SetFileName(const std::string& name) {
	fileName = name;
}

void SourceInfo::SetLine(int line) {
	this->line = line;
}

void SourceInfo::SetColumn(int column) {
	this->column = column;
}

void SourceInfo::SetTotalLocation(const std::string& fileName, int line, int column) {
	this->fileName = fileName;
	this->line = line;
	this->column = column;
}

bool  SourceInfo::operator<(SourceInfo const& loc) const {
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


// Symbol 
ID_T Symbol::GetID() const {
	return id;
}

std::string Symbol::GetName() const {
	return name;
}

ClassType Symbol::GetClassType() const {
	return classType;
}

const SourceInfo& Symbol::GetSourceInfo() const {
	return srcInfo;
}

std::string Symbol::GetNamespace() const {
	return nameSpace;
}

void Symbol::SetID(const ID_T id) {
	this->id = id;
}

void Symbol::SetName(const std::string& name) {
	this->name = name;
}

void Symbol::SetClassType(const ClassType& type) {
	classType = type;
}

void Symbol::SetSourceInfo(const SourceInfo& info) {
	srcInfo = info;
}

void Symbol::SetSourceInfo(const std::string& fileName, int line, int column) {
	srcInfo.SetTotalLocation(fileName, line, column);
}

void Symbol::SetNamespace(const std::string& nameSpace) {
	this->nameSpace = nameSpace;
}


// Template 
template<typename Parent_T> Parent_T* Template<Parent_T>::GetParent() const {
	return parent;
}

template<typename Parent_T> SymbolTable Template<Parent_T>::GetArguments() const {
	return arguments;
}

template<typename Parent_T> void Template<Parent_T>::SetParent(Parent_T* parent) {
	this->parent = parent;
}

template<typename Parent_T> Symbol* Template<Parent_T>::InstallArguments(ID_T id, Structure* structure) {
	return arguments.Install(id, structure);
}


//Definition
const Structure* Definition::GetType() const {
	return type;
}

void Definition::SetType(Structure* structure) {
	type = structure;
}

// Method
MethodType Method::GetMethodType() const {
	return methodType;
}

Structure* Method::GetReturnType() const {
	return returnType;
}

SymbolTable Method::GetArguments() const {
	return arguments;
}

SymbolTable Method::GetDefinitions() const {
	return definitions;
}

SymbolTable Method::GetTemplateArguments() const {
	return templateInfo.GetArguments();
}

std::unordered_map<std::string, Method::MemberExpr>  Method::GetMemberExpr() const {
	return memberExprs;
}

void Method::SetMethodType(const MethodType& type) {
	methodType = type;
}

void Method::SetReturnType(Structure* structure) {
	returnType = structure;
}

void Method::SetTemplateParent(Method* method) {
	templateInfo.SetParent(method);
}

void Method::InstallArg(ID_T id, const Definition& definition) {
	arguments.Install(id, definition);
}

void Method::InstallDefinition(ID_T id, const Definition& definition) {
	definitions.Install(id, definition);
}

void Method::InstallTemplateSpecializationArguments(ID_T id, Structure* structure) {
	templateInfo.InstallArguments(id, structure);
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
Structure::Structure(const Structure& s) {
	id = s.id;
	name = s.name;
	nameSpace = s.nameSpace;
	srcInfo = s.srcInfo;
	classType = s.classType;

	structureType = s.structureType;
	templateInfo = s.templateInfo;
	nestedParent = s.nestedParent;
	methods = s.methods;
	fields = s.fields;
	bases = s.bases;
	contains = s.contains;
	friends = s.friends;
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

SymbolTable Structure::GetMethods() const {
	return methods;
}

SymbolTable Structure::GetFields() const {
	return fields;
}

SymbolTable Structure::GetBases() const {
	return bases;
}

SymbolTable Structure::GetContains() const {
	return contains;
}

SymbolTable Structure::GetFriends() const {
	return friends;
}

SymbolTable Structure::GetTemplateArguments() const {
	return templateInfo.GetArguments(); 
}


void  Structure::SetStructureType(const StructureType& structureType) {
	this->structureType = structureType;
}

void Structure::SetTemplateParent(Structure* structure) {
	templateInfo.SetParent(structure);
}

void Structure::SetNestedParent(Structure* structure) {
	nestedParent = structure;
}

Symbol* Structure::LookupMethod(ID_T id) {
	return methods.Lookup(id);
}

Symbol* Structure::InstallMethod(ID_T id, const Method& method) {
	return methods.Install(id, method);
}

Symbol* Structure::InstallField(ID_T id, const Definition& definition) {
	return fields.Install(id, definition);
}

Symbol* Structure::InstallBase(ID_T id, Structure* structure) {
	return bases.Install(id, structure);
}

Symbol* Structure::InstallNestedClass(ID_T id, Structure* structure) {
	return contains.Install(id, structure);
}

Symbol* Structure::InstallFriend(ID_T id, Structure* structure) {
	return friends.Install(id, structure);
}

Symbol* Structure::InstallTemplateSpecializationArguments(ID_T id, Structure* structure) {
	return templateInfo.InstallArguments(id, structure);
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


// SymbolTable
Symbol* SymbolTable::Install(ID_T id, const std::string& name, const ClassType& type) {
	auto it = byID.find(id);
	if (it != byID.end())
		return it->second;

	Symbol* dummy = nullptr;
	if (type == ClassType::Structure) {
		dummy = new Structure(id, name);
	}
	else if (type == ClassType::Method) {
		dummy = new Method(id, name);
	}
	else if (type == ClassType::Definition) {
		dummy = new Definition(id, name);
	}
	else {
		assert(0);
	}

	byID[id] = dummy;
	auto& nameList = byName[name];
	nameList.push_back(dummy);

	return dummy;
}

Symbol* SymbolTable::Install(ID_T id, const Symbol& symbol) {
	auto it = byID.find(id);
	if (it != byID.end()) {
		if (symbol.GetClassType() == ClassType::Structure && ((Structure*)(it->second))->GetStructureType() == StructureType::Undefined) {
			*(it->second) = symbol;
		}
		return it->second;
	}

	Symbol* newSymbol = new Symbol(symbol);
	byID[id] = newSymbol;

	auto& nameList = byName[symbol.GetName()];
	if (std::find(nameList.begin(), nameList.end(), byID.find(id)->second) == nameList.end()) {
		nameList.push_back(newSymbol);
	}
	return newSymbol;
}


Symbol* SymbolTable::Install(ID_T id, const Structure& symbol) {
	auto it = byID.find(id);
	if (it != byID.end()) {
		if (((Structure*)(it->second))->GetStructureType() == StructureType::Undefined) {	
			(*(Structure*)(it->second)) = symbol;
		}
		return it->second;		
	}

	Symbol* newSymbol = new Structure(symbol);
	byID[id] = newSymbol;
	auto& nameList = byName[symbol.GetName()];
	nameList.push_back(newSymbol);

	return newSymbol;
}

Symbol* SymbolTable::Install(ID_T id, const Method& symbol) {
	auto it = byID.find(id);
	if (it != byID.end()) {
			return it->second;
	}

	Symbol* newSymbol = new Method(symbol);
	byID[id] = newSymbol;
	auto& nameList = byName[symbol.GetName()];
	nameList.push_back(newSymbol);
	return newSymbol;
}

Symbol* SymbolTable::Install(ID_T id, const Definition& symbol) {
	auto it = byID.find(id);
	if (it != byID.end()) 
		return it->second;

	Symbol* newSymbol = new Definition(symbol);
	byID[id] = newSymbol;

	auto& nameList = byName[symbol.GetName()];
	nameList.push_back(newSymbol);

	return newSymbol;
}

Symbol* SymbolTable::Install(ID_T id, Symbol* symbol) {
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

Symbol* SymbolTable::Lookup(ID_T id) {
	auto it = byID.find(id);
	if (it != byID.end())
		return it->second;
	else
		return nullptr;
}


Symbol* SymbolTable::Lookup(const std::string& name) {
	auto it = byName.find(name);
	if (it != byName.end()) {
		//assert(it->second.size() == 1);
		return it->second.front();			
	}
	else
		return nullptr;
}

const Symbol* SymbolTable::Lookup(ID_T id) const {
	auto it = byID.find(id);
	if (it != byID.end())
		return it->second;
	else
		return nullptr;
}


const Symbol* SymbolTable::Lookup(const std::string& name) const{
	auto it = byName.find(name);
	if (it != byName.end()) {
		//assert(it->second.size() == 1);
		return it->second.front();
	}
	else
		return nullptr;
}

void SymbolTable::Print() {
	for (auto& t : byName) {
		std::cout << "Name: " << t.first << std::endl;
		std::cout << "--------------------------------------------\n";
	}
}

