// Contains from string, to dependenciesMining::enums, converter functions.
// Soultatos Stefanos 2022

#pragma once

#include <string>
#include "SymbolTable.h"

namespace incremental::details {

	dependenciesMining::AccessType ToAccessType(const std::string& str);
	
	dependenciesMining::StructureType ToStructureType(const std::string& str);

	dependenciesMining::MethodType ToMethodType(const std::string& str);

} // incremental::details