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

	TEST(ImportStashedST, Imports_nothing_from_non_existent_path) {
		const auto path = ResolvePath("NonExistent.json");
		assert(!std::filesystem::exists(path));
		SymbolTable table;

		ImportStashedST(path.data(), table);

		ASSERT_TRUE(IsEmpty(table));
	}

} // namespace