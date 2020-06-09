#include <list>
#include <string>

class IgnoredNamespaces {
	std::list<std::string> namespaces;

public:
	IgnoredNamespaces() {
		namespaces.push_back("std");
	};
	void Insert(std::string nameSpace);
	void Remove(std::string nameSpace);
	bool isIgnored(std::string nameSpace);
};
