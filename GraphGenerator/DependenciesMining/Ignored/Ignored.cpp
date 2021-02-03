#include "Ignored.h"
#include <assert.h>

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


void IgnoredFilePaths::ReplaceSubStrings(std::string& str, const std::string& replaceThis, const std::string& replaceWith) {
	size_t i = 0;
	auto sub_str_len = replaceThis.length();
	if (sub_str_len == 0)
		return;
	while (true) { // this loop may be improved.
		i = str.find(replaceThis, i);
		if (i == std::string::npos)
			break;
		str.replace(i, sub_str_len, replaceWith);
	}
}

void IgnoredFilePaths::SplitStrWithChar(std::vector<std::string>& splitStr, const std::string& str, const char c) {
	std::string item;
	for (unsigned int i = 0; i < str.length(); i++) {
		if (str[i] == c) {
			splitStr.push_back(item);
			item.clear();
			continue;
		}
		item += str[i];
	}
	if (item != "")
		splitStr.push_back(item);
}

std::stack<std::string> IgnoredFilePaths::GetReverseStack(std::stack<std::string> stack) {
	std::stack<std::string> reversedStack;
	while (!stack.empty()) {
		reversedStack.push(stack.top());
		stack.pop();
	}
	return reversedStack;
}

std::string IgnoredFilePaths::GetFixedPath(const std::vector<std::string>& pathTokens, const char delim) {
	std::string fixedPath = "";
	std::stack<std::string> stack;
	auto len = pathTokens.size();
	assert(len != 0);
	for (auto i : pathTokens) {
		if (i == "..") {
			if (stack.size() != 0)
				stack.pop();
			else
				stack.push(i);
		}
		else
			stack.push(i);
	}
	stack = GetReverseStack(stack);
	while (stack.size() > 1) {
		fixedPath += stack.top();
		fixedPath += delim;
		stack.pop();
	}
	fixedPath += stack.top();
	return fixedPath;
}

std::string IgnoredFilePaths::PathFix(std::string filePath) {
	std::vector<std::string> splitStr;
	ReplaceSubStrings(filePath, "\\", "/");
	SplitStrWithChar(splitStr, filePath, '/');
	return GetFixedPath(splitStr, '/');
}

IgnoredFilePaths::IgnoredFilePaths(const std::string& inputFile) {
	// file that we define to ignore standar lib and some system files (on windows)
	std::string systemIgnoreFilePaths = "..\\Ignore.txt"; 
	std::string fullPath = std::string(__FILE__); 
	std::size_t found = fullPath.find_last_of("/\\");
	std::string line;
	std::ifstream systemFile(fullPath.substr(0, found + 1) + systemIgnoreFilePaths);
	if (systemFile.is_open()) {
		while (std::getline(systemFile, line)) {
			line = PathFix(line);
			Insert(line);
		}
	}

	// user defined inputFile
	if (inputFile == "")
		return;
	std::ifstream userFile(inputFile);
	if (userFile.is_open()) {
		while (std::getline(userFile, line)) {
			line = PathFix(line);
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
