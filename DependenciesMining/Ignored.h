#pragma once
#include <list>
#include <string>
#include <iostream>
#include <fstream>

class Ignored {
protected:
	std::list<std::string> entities;
public:
	virtual void Insert(const std::string& entity);
	virtual void Remove(const std::string& entity);
	virtual bool isIgnored(const std::string& entity) = 0;
};



class IgnoredNamespaces : public Ignored {
public:
	IgnoredNamespaces() {
		Insert("std");
	};
	virtual bool isIgnored(const std::string& nameSpace);
};

class IgnoredFilePaths : public Ignored {
public:
	//IgnoredFilePaths();
	IgnoredFilePaths(const std::string& inputFile = "C:\\Users\\user\\Desktop\\Git\\Architecture-Mining\\DependenciesMining\\Ignore.txt");
	virtual bool isIgnored(const std::string& file);


};