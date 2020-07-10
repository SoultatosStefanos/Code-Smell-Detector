#pragma once
#include <string>

#define ID_T int64_t 


enum class ClassType {
	Undefined = -1,
	Structure,
	Method, 
	Definition
};



class Symbol {
protected:
	ID_T id = -1;
	std::string name;
	std::string nameSpace = "";
	ClassType classType = ClassType::Undefined;

public:
	Symbol() = default;
	Symbol(ClassType classType) : classType(classType) {};
	Symbol(ID_T id, const std::string& name, const std::string& nameSpace = "", ClassType classType = ClassType::Undefined, const std::string& fileName = "", int line = -1, int column = -1)
		: id(id), name(name), classType(classType) {};

	virtual ID_T GetID() const;
	virtual std::string GetName() const;
	virtual ClassType GetClassType() const;
	virtual std::string GetNamespace() const;

	virtual void SetID(const ID_T id);
	virtual void SetName(const std::string& name);
	virtual void SetClassType(const ClassType& type);
	virtual void SetNamespace(const std::string& nameSpace);
};

ID_T Symbol::GetID() const {
	return id;
}

std::string Symbol::GetName() const {
	return name;
}

ClassType Symbol::GetClassType() const {
	return classType;
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

void Symbol::SetNamespace(const std::string& nameSpace) {
	this->nameSpace = nameSpace;
}
