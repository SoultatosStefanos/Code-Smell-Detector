#pragma once
#include <list>
#include <string>
#include <iostream>
#include <fstream>

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
	public:
		IgnoredFilePaths(const std::string& inputFile = "");
		virtual bool isIgnored(const std::string& file);
	};

}