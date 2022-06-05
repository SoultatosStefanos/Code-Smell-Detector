// As it stands, incremental generation would not be possible, since not all symbols are stored in the global symbol table.
// This file contains a workaround.
// Soultatos Stefanos 2022

#pragma once

#include "SymbolTable.h"

namespace incremental {

	void LoadGlobalCache(const dependenciesMining::SymbolTable& from, dependenciesMining::SymbolTable& to);

} // incremental