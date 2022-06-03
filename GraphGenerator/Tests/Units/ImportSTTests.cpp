// Import ST unit tests
// Soultatos Stefanos 2022

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Utility.h"
#include "ImportST.h"
#include <string_view>
#include <cassert>
#include <libgen.h>
#include <filesystem>
#include <random>

namespace {

	using namespace incremental;
	using namespace dependenciesMining;
	using namespace tests::utility;

	// Googletest cannot detect operator<< overloads :(
	#define DUMP_INFO(imported, exported) "\n-----\n" << "\nImported: \n\n" << imported << "\nExported: \n\n" << exported

	TEST(ImportStashedST, Imports_nothing_from_non_existent_path) {
		constexpr auto path = "out.json";
		assert(!std::filesystem::exists(path));
		SymbolTable table;

		ImportStashedST(path, table);

		ASSERT_TRUE(table.IsEmpty());
	}

	TEST(ImportStashedST, Imports_empty_symbol_table_correctly) {
		const auto path = RESOLVE_PATH("sample_empty.json");
		assert(std::filesystem::exists(path));
		SymbolTable table;

		ImportStashedST(path, table);

		ASSERT_TRUE(table.IsEmpty());
	}

	// TODO Imports_non_empty_symbol_table_correctly

} // namespace