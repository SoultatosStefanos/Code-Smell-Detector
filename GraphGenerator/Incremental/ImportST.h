#pragma once

#include "SymbolTable.h"
#include <fstream>
#include <string_view>

namespace incremental {

	using JsonArchive = std::ifstream;
	void ImportST(JsonArchive& from, dependenciesMining::SymbolTable& table);

	void ImportSTIfCached(const std::string_view fpath, dependenciesMining::SymbolTable& table);
}