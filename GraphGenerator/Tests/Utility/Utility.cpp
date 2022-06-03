// Contains testing utilities.
// Soultatos Stefanos 2022

#include "Utility.h"

namespace tests::utility {

	std::string ResolvePath(std::string from, const std::string& to) {
		const auto file_directory = std::string(dirname(from.data()));
		auto resolved_path = file_directory + "/" + to;
	
		assert((!from.empty() or !to.empty()) ? resolved_path.size() >= 2 : true);
		return resolved_path;
	}

} // tests::utils