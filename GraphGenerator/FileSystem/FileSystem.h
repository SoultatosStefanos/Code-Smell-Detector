// Filesystem utilities.
// Soultatos Stefanos 2022

#include <string>

namespace filesystem {

	bool IsHeaderFile(const std::string& fileName);
	bool IsSourceFile(const std::string& fileName);

} // filesystem