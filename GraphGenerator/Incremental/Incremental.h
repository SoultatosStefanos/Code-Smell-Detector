// Incremental Generation module.
// Soultatos Stefanos 2022

#pragma once

#include "SymbolTable.h"
#include <fstream>
#include <string_view>
#include <jsoncpp/json/json.h>
#include <vector>
#include <string_view>

namespace incremental {

	using Sources = std::vector<std::string>;

	void ImportST(const std::string_view jsonPath, dependenciesMining::SymbolTable& table);
	void ImportSources(const std::string_view jsonPath, Sources& srcs);

} // incremental