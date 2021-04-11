#include "SymbolTable.h"
#include "STVisitor.h"

using namespace dependenciesMining;

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

bool SourceInfo::operator==(SourceInfo const& loc) const {
	assert(fileName == loc.fileName);
	if (line == loc.line && column == loc.column)
		return true;
	else
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

std::string Symbol::GetClassTypeAsString() const {
	if (classType == ClassType::Structure) {
		return "Structure";
	}
	else if(classType == ClassType::Method){
		return "Method";
	}
	else if (classType == ClassType::Definition) {
		return "Definition";
	}
	else {
		assert(0);
	}
}


const SourceInfo& Symbol::GetSourceInfo() const {
	return srcInfo;
}

std::string Symbol::GetNamespace() const {
	return nameSpace;
}

const char* Symbol::GetAccessTypeStr() const {
	switch (access_type) {
	case AccessType::_public:
		return "public";
	case AccessType::_protected:
		return "protected";
	case AccessType::_private:
		return "private";
	default:
		return "unknown";
	}
}

void Symbol::SetID(const ID_T& id) {
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

void Symbol::SetAccessType(const AccessType& access_type) {
	this->access_type = access_type;
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

template<typename Parent_T> Symbol* Template<Parent_T>::InstallArguments(const ID_T& id, Structure* structure) {
	return arguments.Install(id, structure);
}


//Definition
bool Definition::isStructure() const {
	return type != nullptr;
}


const Structure* Definition::GetType() const {
	return type;
}

std::string Definition::GetFundamental() const {
	return fundamental;
}

void Definition::SetType(Structure* structure) {
	type = structure;
}

void Definition::SetFundamental(const std::string& type) {
	fundamental = type;
}

// Method
MethodType Method::GetMethodType() const {
	return methodType;
}

std::string Method::GetMethodTypeAsString() const {
	if (methodType == MethodType::Constructor_UserDefined) {
		return "Constructor_UserDefined";
	}
	else if (methodType == MethodType::Constructor_Trivial) {
		return "Constructor_Trivial";
	}
	else if (methodType == MethodType::Destructor_UserDefined) {
		return "Destructor_UserDefined";
	}
	else if (methodType == MethodType::Destructor_Trivial) {
		return "Destructor_Trivial";
	}
	else if (methodType == MethodType::OverloadedOperator_UserDefined) {
		return "OverloadedOperator_UserDefined";
	}
	else if (methodType == MethodType::OverloadedOperator_Trivial) {
		return "OverloadedOperator_Trivial";
	}
	else if (methodType == MethodType::UserMethod) {
		return "UserMethod";
	}
	else if (methodType == MethodType::TemplateDefinition) {
		return "TemplateDefinition";
	}
	else if (methodType == MethodType::TemplateFullSpecialization) {
		return "TemplateFullSpecialization";
	}
	else if (methodType == MethodType::TemplateInstantiationSpecialization) {
		return "TemplateInstantiationSpecialization";
	}
	else {
		assert(0); 
	}
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

std::map<std::string, Method::MemberExpr>  Method::GetMemberExpr() const {
	return memberExprs;
}

int Method::GetLiterals() const {
	return literals;
}

int Method::GetStatements() const {
	return statements;
}

int Method::GetBranches() const {
	return branches;
}

int Method::GetLoops() const {
	return loops;
}

int Method::GetMaxScopeDepth() const {
	return max_scope_depth;
}

int Method::GetLineCount() const {
	return line_count;
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

void Method::SetLiterals(int literals) {
	this->literals = literals;
}

void Method::SetStatements(int statements) {
	this->statements = statements;
}

void Method::SetBranches(int branches) {
	this->branches = branches;
}

void Method::SetLoops(int loops) {
	this->loops = loops;
}

void Method::SetMaxScopeDepth(int max_scope_depth) {
	this->max_scope_depth = max_scope_depth;
}

void Method::SetLineCount(int line_count) {
	this->line_count = line_count;
}

void Method::InstallArg(const ID_T& id, const Definition& definition) {
	arguments.Install(id, definition);
}

void Method::InstallDefinition(const ID_T& id, const Definition& definition) {
	definitions.Install(id, definition);
}

void Method::InstallTemplateSpecializationArguments(const ID_T& id, Structure* structure) {
	templateInfo.InstallArguments(id, structure);
}

void Method::InsertMemberExpr(MemberExpr const& memberExpr, Member const& member, const std::string& locBegin) {
	//if (memberExpr.GetExpr() != "__$Ignore__") {
		if (memberExprs.find(locBegin) == memberExprs.end()) {
			memberExprs[locBegin] = memberExpr;
		}
		else {
			if (memberExpr.GetLocEnd() > memberExprs[locBegin].GetLocEnd()) {
				memberExprs[locBegin].SetExpr(memberExpr.GetExpr());
				memberExprs[locBegin].SetLocEnd(memberExpr.GetLocEnd());
			}
		}
	//}
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

bool Method::IsConstructor() const {
	if (methodType == MethodType::Constructor_UserDefined || methodType == MethodType::Constructor_Trivial)
		return true; 
	return false;
}

bool Method::IsDestructor() const {
	if (methodType == MethodType::Destructor_UserDefined || methodType == MethodType::Destructor_Trivial)
		return true;
	return false;
}

bool Method::IsOverloadedOperator() const {
	if (methodType == MethodType::OverloadedOperator_UserDefined || methodType == MethodType::OverloadedOperator_Trivial)
		return true;
	return false;
}

bool Method::IsUserMethod() const {
	if (methodType == MethodType::UserMethod)
		return true;
	return false;
}

bool Method::IsTemplateDefinition() const {
	if (methodType == MethodType::TemplateDefinition)
		return true;
	return false;
}

bool Method::IsTemplateFullSpecialization() const {
	if (methodType == MethodType::TemplateFullSpecialization)
		return true;
	return false;
}

bool Method::IsTemplateInstantiationSpecialization() const {
	if (methodType == MethodType::TemplateInstantiationSpecialization)
		return true;
	return false;
}

bool Method::IsTrivial() const {
	if (methodType == MethodType::Constructor_Trivial ||
		methodType == MethodType::OverloadedOperator_Trivial ||
		methodType == MethodType::Destructor_Trivial) {
		return true;
	}
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

Method::Member::MemberType Method::Member::GetMemberType() const {
	return memType;
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

SourceInfo Method::MemberExpr::GetSourceInfo() const {
	return srcInfo;
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

std::string Structure::GetStructureTypeAsString() const{
	if (structureType == StructureType::Class) {
		return "Class";
	}
	else if (structureType == StructureType::Struct) {
		return "Struct";
	}
	else if (structureType == StructureType::TemplateDefinition) {
		return "TemplateDefinition";
	}
	else if (structureType == StructureType::TemplateFullSpecialization) {
		return "TemplateFullSpecialization";
	}
	else if (structureType == StructureType::TemplateInstantiationSpecialization) {
		return "TemplateInstantiationSpecialization";
	}
	else if (structureType == StructureType::TemplatePartialSpecialization) {
		return "TemplatePartialSpecialization";
	}
	else {
		return "Undefined";
	}
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

Symbol* Structure::LookupMethod(const ID_T& id) {
	return methods.Lookup(id);
}

Symbol* Structure::InstallMethod(const ID_T& id, const Method& method) {
	return methods.Install(id, method);
}

Symbol* Structure::InstallField(const ID_T& id, const Definition& definition) {
	return fields.Install(id, definition);
}

Symbol* Structure::InstallBase(const ID_T& id, Structure* structure) {
	return bases.Install(id, structure);
}

Symbol* Structure::InstallNestedClass(const ID_T& id, Structure* structure) {
	return contains.Install(id, structure);
}

Symbol* Structure::InstallFriend(const ID_T& id, Structure* structure) {
	return friends.Install(id, structure);
}

Symbol* Structure::InstallTemplateSpecializationArguments(const ID_T& id, Structure* structure) {
	return templateInfo.InstallArguments(id, structure);
}

bool Structure::IsTemplateDefinition() const {
	if (structureType == StructureType::TemplateDefinition)
		return true;
	return false;
}

bool Structure::IsTemplateFullSpecialization() const {
	if (structureType == StructureType::TemplateFullSpecialization)
		return true;
	return false;
}

bool Structure::IsTemplateInstantiationSpecialization() const {
	if (structureType == StructureType::TemplateInstantiationSpecialization)
		return true;
	return false;
}

bool Structure::IsTemplatePartialSpecialization() const {
	if (structureType == StructureType::TemplatePartialSpecialization)
		return true;
	return false;
}

bool Structure::IsTemplate() const {
	if (structureType >= StructureType::TemplateDefinition && structureType <= StructureType::TemplatePartialSpecialization)
		return true;
	return false;
}

bool Structure::IsUndefined() const {
	if (structureType == StructureType::Undefined)
		return true;
	return false;
}

bool Structure::IsNestedClass() const {
	if (nestedParent)
		return true;
	return false;
}


// SymbolTable
Symbol* SymbolTable::Install(const ID_T& id, const std::string& name, const ClassType& type) {
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

Symbol* SymbolTable::Install(const ID_T& id, const Symbol& symbol) {
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


Symbol* SymbolTable::Install(const ID_T& id, const Structure& symbol) {
	auto it = byID.find(id);
	if (it != byID.end()) {
		if (((Structure*)(it->second))->GetStructureType() == StructureType::Undefined) {	
			(*(Structure*)(it->second)) = symbol;
		}
		else {							// Ignpre it, only for debugging use
			return it->second;			//
		}								//
		return it->second;		
	}

	Symbol* newSymbol = new Structure(symbol);
	byID[id] = newSymbol;
	auto& nameList = byName[symbol.GetName()];
	nameList.push_back(newSymbol);

	return newSymbol;
}

Symbol* SymbolTable::Install(const ID_T& id, const Method& symbol) {
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

Symbol* SymbolTable::Install(const ID_T& id, const Definition& symbol) {
	auto it = byID.find(id);
	if (it != byID.end()) 
		return it->second;

	Symbol* newSymbol = new Definition(symbol);
	byID[id] = newSymbol;

	auto& nameList = byName[symbol.GetName()];
	nameList.push_back(newSymbol);

	return newSymbol;
}

Symbol* SymbolTable::Install(const ID_T& id, Symbol* symbol) {
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

Symbol* SymbolTable::Lookup(const ID_T& id) {
	auto it = byID.find(id);
	if (it != byID.end())
		return it->second;
	else
		return nullptr;
}


//Symbol* SymbolTable::Lookup(const std::string& name) {
//	auto it = byName.find(name);
//	if (it != byName.end()) {
//		//assert(it->second.size() == 1);
//		return it->second.front();			
//	}
//	else
//		return nullptr;
//}

const Symbol* SymbolTable::Lookup(const ID_T& id) const {
	auto it = byID.find(id);
	if (it != byID.end())
		return it->second;
	else
		return nullptr;
}


//const Symbol* SymbolTable::Lookup(const std::string& name) const{
//	auto it = byName.find(name);
//	if (it != byName.end()) {
//		//assert(it->second.size() == 1);
//		return it->second.front();
//	}
//	else
//		return nullptr;
//}

static Json::Value GetJsonSourceInfo(Symbol* symbol) {
	Json::Value json_src_info;
	auto src_info = symbol->GetSourceInfo();
	json_src_info["file"] = src_info.GetFileName();
	json_src_info["line"] = src_info.GetLine();
	json_src_info["column"] = src_info.GetColumn();

	return json_src_info;
}

void SymbolTable::Print() {
	for (auto& t : byName) {
		std::cout << "Name: " << t.first << std::endl;
		std::cout << "--------------------------------------------\n";
	}
}

// Testing purpose.
void SymbolTable::Print2(int level) {
	for (auto& t : byID) {
		std::cout << "level " << level << ", Name: " << t.first << std::endl;
		//std::cout << "--------------------------------------------\n";
		if (t.second->GetClassType() == ClassType::Structure) {
			Structure* structure = (Structure*)t.second;
			structure->GetMethods().Print2(level + 1);
			structure->GetFields().Print2(level + 1);
			structure->GetBases().Print2(level + 1);
			structure->GetContains().Print2(level + 1);
			structure->GetFriends().Print2(level + 1);
		}
		else if (t.second->GetClassType() == ClassType::Method) {
			Method* method = (Method*)t.second;
			std::cout << "args: ---------\n";
			method->GetArguments().Print2(level + 1);
			std::cout << "end : ---------\n";
			method->GetDefinitions().Print2(level + 1);
			method->GetTemplateArguments().Print2(level + 1);
			const auto& member_expr = method->GetMemberExpr();
			for (auto& i : member_expr) {
				std::cout << "Member EXPR: " << i.first << std::endl;
				for (auto& it : i.second.GetMembers()) {
					std::cout << "Member: " << it.GetName() << std::endl;
				}
			}

		}
	}
}

Json::Value SymbolTable::GetJsonStructure(dependenciesMining::Structure* structure) {
	Json::Value json_structure;


	json_structure["methods"] = structure->GetMethods().GetJsonSymbolTable();
	json_structure["fields"] = structure->GetFields().GetJsonSymbolTable();
	json_structure["bases"] = structure->GetBases().GetJsonSymbolTable();
	json_structure["contains"] = structure->GetContains().GetJsonSymbolTable();
	json_structure["friends"] = structure->GetFriends().GetJsonSymbolTable();
	json_structure["src_info"] = GetJsonSourceInfo(structure);

	return json_structure;
}

Json::Value SymbolTable::GetJsonMethod(dependenciesMining::Method* method) {
	Json::Value json_method;

	/*auto iss = method->GetMemberExpr();
	std::cout << iss.size() << std::endl;
	for (auto is : iss) {
		std::cout << is.first << std::endl;
	}*/
	auto* ret_type = method->GetReturnType();
#pragma warning ("FIX ME!!!!")
	if (!ret_type)
		json_method["ret_type"] = "void";
	else
		json_method["ret_type"] = ret_type->GetID();
	json_method["args"] = method->GetArguments().GetJsonSymbolTable();
	json_method["definitions"] = method->GetDefinitions().GetJsonSymbolTable();
	json_method["template_args"] = method->GetTemplateArguments().GetJsonSymbolTable();
	json_method["literals"] = method->GetLiterals();
	json_method["statements"] = method->GetStatements();
	json_method["branches"] = method->GetBranches();
	json_method["loops"] = method->GetLoops();
	json_method["src_info"] = GetJsonSourceInfo(method);
	json_method["max_scope"] = method->GetMaxScopeDepth();
	json_method["lines"] = method->GetLineCount();
	json_method["access"] = method->GetAccessTypeStr();

#pragma warning(">>>>>>>>>>>>>> GetMemberExpr() <<<<<<<<<<<<<<<<<")
	return json_method;
}

Json::Value SymbolTable::GetJsonDefinition(dependenciesMining::Definition* definition) {
	Json::Value json_definition;
	if (definition->isStructure())
		json_definition["type"] = definition->GetType()->GetID();
	else // is fundamental
		json_definition["type"] = definition->GetFundamental();

	return json_definition;
}

Json::Value SymbolTable::GetJsonSymbolTable(void) {
	Json::Value vec(Json::arrayValue);

	for (auto& t : byID) {
		Json::Value new_obj;

		if (t.second->GetClassType() == ClassType::Structure) {
			new_obj = GetJsonStructure((dependenciesMining::Structure*)t.second);
		}
		else if (t.second->GetClassType() == ClassType::Definition) {
			new_obj = GetJsonDefinition((dependenciesMining::Definition*)t.second);
		}
		else if (t.second->GetClassType() == ClassType::Method) {
			new_obj = GetJsonMethod(((dependenciesMining::Method*)t.second));
		}
		else if (t.second->GetClassType() == ClassType::Undefined) {
			// new_obj = ...
		}
		else
			assert(0);
		new_obj["id"] = t.second->GetID();
		vec.append(new_obj);
	}

	return vec;
}

void SymbolTable::AddJsonStructure(dependenciesMining::Structure* structure, Json::Value &json_structure) {
	//Json::Value json_structure;


	structure->GetMethods().AddJsonSymbolTable(json_structure["methods"]);
	structure->GetFields().AddJsonSymbolTable(json_structure["fields"]);
	const auto bases = structure->GetBases();
	for (const auto& base : bases) {
		json_structure["bases"].append(base.second->GetID());
	}
	//structure->GetBases().AddJsonSymbolTable(json_structure["bases"]); // FIXME need only id
	structure->GetContains().AddJsonSymbolTable(json_structure["contains"]);
	structure->GetFriends().AddJsonSymbolTable(json_structure["friends"]);
	json_structure["src_info"] = GetJsonSourceInfo(structure);
}

void SymbolTable::AddJsonMethod(dependenciesMining::Method* method, Json::Value &json_method) {
	//Json::Value json_method;

	/*auto iss = method->GetMemberExpr();
	std::cout << iss.size() << std::endl;
	for (auto is : iss) {
		std::cout << is.first << std::endl;
	}*/
	auto* ret_type = method->GetReturnType();
#pragma warning ("FIX ME!!!!")
	if (!ret_type)
		json_method["ret_type"] = "void";
	else
		json_method["ret_type"] = ret_type->GetID();
	method->GetArguments().AddJsonSymbolTable(json_method["args"]);
	method->GetDefinitions().AddJsonSymbolTable(json_method["definitions"]);
	method->GetTemplateArguments().AddJsonSymbolTable(json_method["template_args"]);
	json_method["literals"] = method->GetLiterals();
	json_method["statements"] = method->GetStatements();
	json_method["branches"] = method->GetBranches();
	json_method["loops"] = method->GetLoops();
	json_method["src_info"] = GetJsonSourceInfo(method);
	json_method["max_scope"] = method->GetMaxScopeDepth();
	json_method["lines"] = method->GetLineCount();
	json_method["access"] = method->GetAccessTypeStr();


#pragma warning(">>>>>>>>>>>>>> GetMemberExpr() <<<<<<<<<<<<<<<<<")
}

void SymbolTable::AddJsonDefinition(dependenciesMining::Definition* definition, Json::Value& json_definition) {
	if (definition->isStructure())
		json_definition["type"] = definition->GetType()->GetID();
	else // is fundamental
		json_definition["type"] = definition->GetFundamental();

}

void SymbolTable::AddJsonSymbolTable(Json::Value& st) {

	for (auto& t : byID) {
		Json::Value new_obj;

		if (t.second->GetClassType() == ClassType::Structure) {
			 AddJsonStructure((dependenciesMining::Structure*)t.second, new_obj);
		}
		else if (t.second->GetClassType() == ClassType::Definition) {
			 AddJsonDefinition((dependenciesMining::Definition*)t.second, new_obj);
		}
		else if (t.second->GetClassType() == ClassType::Method) {
			 AddJsonMethod((dependenciesMining::Method*)t.second, new_obj);
		}
		else if (t.second->GetClassType() == ClassType::Undefined) {
			// new_obj = ...
		}
		else
			assert(0);

		st[t.second->GetID()] = new_obj;
	}
}


void SymbolTable::Accept(STVisitor* visitor) {
	for (auto it : byID) {
		auto* symbol = it.second;
		if (symbol->GetClassType() == ClassType::Structure) {
			visitor->VisitStructure((Structure*)symbol);
		}
		else if (symbol->GetClassType() == ClassType::Method) {
			visitor->VisitMethod((Method*)symbol);
		}
		else if (symbol->GetClassType() == ClassType::Definition) {
			visitor->VisitDefinition((Definition*)symbol);
		}
		else {
			assert(0);
		}
	}
}


void SymbolTable::Accept(STVisitor* visitor) const {
	for (auto it : byID) {
		auto* symbol = it.second;
		if (symbol->GetClassType() == ClassType::Structure) {
			visitor->VisitStructure((Structure*)symbol);
		}
		else if (symbol->GetClassType() == ClassType::Method) {
			visitor->VisitMethod((Method*)symbol);
		}
		else if (symbol->GetClassType() == ClassType::Definition) {
			visitor->VisitDefinition((Definition*)symbol);
		}
		else {
			assert(0);
		}
	}
}
