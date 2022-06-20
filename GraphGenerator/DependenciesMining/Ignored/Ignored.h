#pragma once

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <memory>

namespace dependenciesMining {
	
	class Ignored {
	protected:
		std::list<std::string> entities;
	public:
		virtual void Insert(const std::string& entity);
		virtual void Remove(const std::string& entity);
		virtual bool isIgnored(const std::string& entity) = 0;
	};

	// --------------------------------------------------------------------

	class IgnoredNamespaces : public Ignored {
	public:
		IgnoredNamespaces(const std::string& inputFile = ""); 
		virtual bool isIgnored(const std::string& nameSpace);
	};

	// --------------------------------------------------------------------

	class IgnoredFilePaths : public Ignored {
	private:
		void ReplaceSubStrings(std::string& str, const std::string& replaceThis, const std::string& replaceWith);
		void SplitStrWithChar(std::vector<std::string>& splitStr, const std::string& str, const char c);
		void ReverseStack(std::stack<std::string>& stack);
		std::string GetFixedPath(const std::vector<std::string>& splitStr, const char delim);
		std::string PathFix(std::string filePath);
	public:
		IgnoredFilePaths(const std::string& inputFile = "");
		virtual bool isIgnored(const std::string& file);
	};

}