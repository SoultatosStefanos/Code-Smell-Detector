// Contains incremetnal integration tests
// Soultatos Stefanos 2022

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "DependenciesMining.h"
#include "SymbolTable.h"
#include "Utility.h"
#include "ImportST.h"

#include <cassert>
#include <filesystem>


namespace {

	using namespace dependenciesMining;
	using namespace tests::utility;
	using namespace incremental;

	// Googletest cannot detect operator<< overloads :(
	#define DUMP_INFO(exported, imported) "\n-----\n" << "\nExported: \n\n" << exported << "\nImported: \n\n" << imported

	class IncrementalTests : public testing::Test {
	protected:
		void TearDown() override { structuresTable.Clear(); } // because global
	};

	void ExportST(SymbolTable& table, const std::string_view jsonPath) {
		assert(!std::filesystem::exists(jsonPath));

		Json::Value jsonST;
		table.AddJsonSymbolTable(jsonST["structures"]);
		std::ofstream jsonSTFile{jsonPath.data()};
		jsonSTFile << jsonST;

		assert(std::filesystem::exists(jsonPath));
	}

	TEST_F(IncrementalTests, Importing_after_exporting_ST) {
		const auto tmp = RESOLVE_PATH("out.json");
		const auto cmp_db = RESOLVE_PATH("compile_commands.json");
		std::vector<std::string> srcs;
		std::vector<std::string> headers;
		CreateClangTool(cmp_db.c_str(), srcs, headers, "", "");
		ExportST(structuresTable, tmp);

		SymbolTable imported;
		ImportStashedST(tmp.c_str(), imported);

		EXPECT_EQ(imported, structuresTable) << DUMP_INFO(structuresTable, imported);

		std::remove(tmp.c_str());
	}


} // namespace


