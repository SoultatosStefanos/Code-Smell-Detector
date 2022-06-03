#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ImportST.h"
#include <string_view>
#include <cassert>
#include <libgen.h>
#include <filesystem>

namespace {

	using namespace incremental;
	using namespace dependenciesMining;

	// Useful for out-of-source builds.
	inline auto ResolvePath(const std::string& to, std::string from = __FILE__) {
		const auto file_directory = std::string(dirname(from.data()));
		auto resolved_path = file_directory + "/" + to;

		assert((!from.empty() or !to.empty()) ? resolved_path.size() >= 2 : true);
		return resolved_path;
	}

	inline bool IsEmpty(const SymbolTable& table) {
		const auto countAll = [](const auto&) { return true; };
		return std::count_if(std::begin(table), std::end(table), countAll) == 0;
	}

	inline void ExportST(SymbolTable& table,const std::string_view jsonPath) {
		assert(!std::filesystem::exists(jsonPath));

		Json::Value jsonST;
		table.AddJsonSymbolTable(jsonST["structures"]);
		std::ofstream jsonSTFile{jsonPath.data()};
		jsonSTFile << jsonST;

		assert(std::filesystem::exists(jsonPath));
	}

	// RAI With temp file
	struct TempFile {
		TempFile(const std::string_view fname) : file{fname} {}
		~TempFile() { std::remove(file.data()); }

		operator std::string_view() const {return file;} 

		const std::string_view file;
	};

	[[nodiscard]] inline auto MakeTempFile(const std::string_view fname) { return TempFile{fname}; }

	TEST(ImportStashedST, Imports_nothing_from_non_existent_path) {
		const auto path = ResolvePath("Out.json");
		assert(!std::filesystem::exists(path));
		SymbolTable table;

		ImportStashedST(path, table);

		ASSERT_TRUE(IsEmpty(table));
	}

	TEST(ImportStashedST, Imports_nothing_from_empty_json) {
		const auto tmp = MakeTempFile(ResolvePath("Out.json"));
		SymbolTable table;
		ExportST(table, tmp);

		ImportStashedST(tmp, table);

		ASSERT_TRUE(IsEmpty(table));
	}

} // namespace