#include <cassert>
#include <iostream>

#include "FileSystem.h"

namespace filesystem {

	namespace {

		// returns true only if str ends with ending
		inline bool EndsWith(std::string const& str, std::string const& ending) { // TODO Optimize
			if (str.length() >= ending.length())
				return (0 == str.compare(str.length() - ending.length(), ending.length(), ending));
			return false;
		}

	} // namespace

	bool IsHeaderFile(const std::string& fileName) { // TODO Optimize
		return EndsWith(fileName, ".h") or EndsWith(fileName, ".hpp"); // TODO More
	}
 
	bool IsSourceFile(const std::string& fileName) { // TODO Optimize
		return EndsWith(fileName, ".cpp") or EndsWith(fileName, ".cc"); // TODO More
	}

} // filesystem