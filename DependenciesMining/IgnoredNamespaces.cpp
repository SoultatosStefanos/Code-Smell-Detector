#include "IgnoredNamespaces.h"

void IgnoredNamespaces::Insert(std::string nameSpace) {
	namespaces.push_back(nameSpace);
}

void IgnoredNamespaces::Remove(std::string nameSpace) {
	auto it = std::find(namespaces.begin(), namespaces.end(), nameSpace);
	if (it != namespaces.end())
		namespaces.erase(it);
}

bool IgnoredNamespaces::isIgnored(std::string nameSpace) {
	auto it = std::find(namespaces.begin(), namespaces.end(), nameSpace);
	if (it != namespaces.end())
		return true;
	return false;
}


// Given namespace Path 
/*bool IgnoredNamespaces::isIgnored(std::string nameSpace) {
	for (auto it : namespaces) {
		if (nameSpace.find(it + "::") != std::string::npos) {
			return true;
		}
	}
	return false;
}*/
