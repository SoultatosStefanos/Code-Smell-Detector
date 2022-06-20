// Incremental Generation module.
// Soultatos Stefanos 2022

#pragma once

#include "SymbolTable.h"

#include <clang/Tooling/Tooling.h>
#include <fstream>
#include <string_view>
#include <jsoncpp/json/json.h>
#include <vector>
#include <string_view>

namespace incremental {

	using Sources = std::vector<std::string>;
	using SourceIDs = std::vector<unsigned>;

	void ImportST(const std::string_view jsonPath, dependenciesMining::SymbolTable& table);
	void ImportSources(const std::string_view jsonPath, Sources& srcs);
	void ImportSourceIDs(const std::string_view jsonPath, SourceIDs& srcs);

	// Assumes that all cached files, except the last one, were entirely parsed.
	Sources DropParsedFiles(const Sources& sources, const SourceIDs& cached_ids);

	void SerializeSourceIDs(Json::Value& value, const Sources& srcs, clang::FileManager& manager);

} // incremental