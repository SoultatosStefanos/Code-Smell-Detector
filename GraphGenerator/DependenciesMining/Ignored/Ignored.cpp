#include "Ignored.h"

using namespace dependenciesMining;

void Ignored::Insert(const std::string& entity) {
	auto it = std::find(entities.begin(), entities.end(), entity);
	if (it == entities.end())
		entities.push_back(entity);
}

void Ignored::Remove(const std::string& entity) {
	auto it = std::find(entities.begin(), entities.end(), entity);
	if (it != entities.end())
		entities.erase(it);
}

// --------------------------------------------------------------------

IgnoredNamespaces::IgnoredNamespaces(const std::string& inputFile)  {
	Insert("std");
	if (inputFile == "")
		return;
	std::string line;
	std::ifstream file(inputFile);
	if (file.is_open()) {
		while (std::getline(file, line)) {
			Insert(line);
		}
	}
};

bool IgnoredNamespaces::isIgnored(const std::string& nameSpace) {
	for (auto n : entities) {
		if (nameSpace.find(n) == 0) {
			return true;
		}
	}
	return false;
}

// --------------------------------------------------------------------

IgnoredFilePaths::IgnoredFilePaths(const std::string& inputFile) {
	// file that we define to ignore standar lib and some system files (on windows)
	std::string systemIgnoreFilePaths = "..\\Ignore.txt"; 
	std::string fullPath = std::string(__FILE__); 
	std::size_t found = fullPath.find_last_of("/\\");
	std::string line;
	std::ifstream systemFile(fullPath.substr(0, found + 1) + systemIgnoreFilePaths);
	if (systemFile.is_open()) {
		while (std::getline(systemFile, line)) {
			Insert(line);
		}
	}

	// user defined inputFile
	if (inputFile == "")
		return;
	std::ifstream userFile(inputFile);
	if (userFile.is_open()) {
		while (std::getline(userFile, line)) {
			Insert(line);
		}
	}
}

bool IgnoredFilePaths::isIgnored(const std::string& file) {
	for (auto n : entities) {
		if (file.find(n) <= file.length()) {
			return true;
		}
	}
	return false;
}
