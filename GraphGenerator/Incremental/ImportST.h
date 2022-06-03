// Contains functions to import a Symbol Table from a json file.
// Soultatos Stefanos 2022

#pragma once

#include "SymbolTable.h"
#include <fstream>
#include <string_view>

namespace incremental {

	using JsonArchive = std::ifstream;

	void ImportST(JsonArchive& from, dependenciesMining::SymbolTable& table);
	void ImportStashedST(const std::string_view fpath, dependenciesMining::SymbolTable& table); // Checks for file existence.
	
} // incremental