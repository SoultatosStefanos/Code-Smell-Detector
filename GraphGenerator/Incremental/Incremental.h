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

	void ImportST(const std::string_view jsonPath, dependenciesMining::SymbolTable& table);
	void ImportSources(const std::string_view jsonPath, std::vector<std::string>& srcs);

	void LoadGlobalCache(const dependenciesMining::SymbolTable& from, dependenciesMining::SymbolTable& to); // TODO Remove

}