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

	std::string_view cmp_db;

	class IncrementalTests : public testing::Environment {
	public:
  		explicit IncrementalTests(const std::string_view database) {
			cmp_db = database;
		}
	};

	void ExportST(const SymbolTable& table, const std::string_view jsonPath) {
		assert(!std::filesystem::exists(jsonPath));

		Json::Value jsonST;
		table.AddJsonSymbolTable(jsonST["structures"]);
		std::ofstream jsonSTFile{jsonPath.data()};
		jsonSTFile << jsonST;

		assert(std::filesystem::exists(jsonPath));
	}

	TEST(IncrementalTests, Importing_after_exporting_ST) {
		constexpr auto tmp = "out.json";
		std::vector<std::string> srcs;
		std::vector<std::string> headers;
		CreateClangTool(cmp_db.data(), srcs, headers, "", "");
		ExportST(structuresTable, tmp);

		SymbolTable imported;
		ImportStashedST(tmp, imported);

		EXPECT_TRUE(AreEqual(imported, structuresTable));

		std::remove(tmp);
	}


} // namespace

int main(int argc, char* argv[]) {
    try {
		if (argc != 2)
			throw std::runtime_error{"Usage: ./IncrementalTests <Compilation Database>"};

        ::testing::InitGoogleTest(&argc, argv);
		::testing::AddGlobalTestEnvironment(new IncrementalTests(argv[1]));

        return RUN_ALL_TESTS();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';

        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "unexpected error\n";

        return EXIT_FAILURE;
    }
}

