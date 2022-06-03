#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ImportST.h"
#include <string_view>
#include <cassert>
#include <libgen.h>
#include <filesystem>
#include <random>

namespace {

	using namespace incremental;
	using namespace dependenciesMining;

	template <typename Seed = std::random_device>
	auto Rng() -> auto&
	{
		thread_local static std::mt19937 generator{Seed{}()};
		return generator;
	}

	// Uniform random function, seeded by thread local generator.
	template <typename T>
	T URandom(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
	{
		static_assert(std::is_integral_v<T> or std::is_floating_point_v<T>);

		using Distribution = std::conditional_t<std::is_integral_v<T>,
										std::uniform_int_distribution<T>,
										std::uniform_real_distribution<T>>;

		Distribution dist{min, max};
		return static_cast<T>(dist(Rng()));
	}

	template <typename T, typename Function>
	inline void Repeat(T times, Function f) {
		static_assert(std::is_integral_v<T> or std::is_floating_point_v<T>);

		for (T i = 0; i < times; ++i)
			f();
	}

	// Useful for out-of-source builds.
	inline auto ResolvePath(const std::string& to, std::string from = __FILE__) {
		const auto file_directory = std::string(dirname(from.data()));
		auto resolved_path = file_directory + "/" + to;

		assert((!from.empty() or !to.empty()) ? resolved_path.size() >= 2 : true);
		return resolved_path;
	}

	inline void ExportST(SymbolTable& table, const std::string_view jsonPath) {
		assert(!std::filesystem::exists(jsonPath));

		Json::Value jsonST;
		table.AddJsonSymbolTable(jsonST["structures"]);
		std::ofstream jsonSTFile{jsonPath.data()};
		jsonSTFile << jsonST;

		assert(std::filesystem::exists(jsonPath));
	}

	// Googletest cannot detect operator<< overloads :(
	#define DUMP_INFO(imported, exported) "\n-----\n" << "\nImported: \n\n" << imported << "\nExported: \n\n" << exported

	TEST(ImportStashedST, Imports_nothing_from_non_existent_path) {
		const auto path = ResolvePath("Out.json");
		assert(!std::filesystem::exists(path));
		SymbolTable table;

		ImportStashedST(path, table);

		ASSERT_TRUE(table.IsEmpty());
	}

	TEST(ImportStashedST, Imports_empty_symbol_table_correctly) {
		const auto tmp = ResolvePath("Out.json");
		SymbolTable table;
		ExportST(table, tmp);

		ImportStashedST(tmp, table);

		EXPECT_TRUE(table.IsEmpty());

		std::remove(tmp.data());
	}

	TEST(ImportStashedST, Imports_one_empty_structure_correctly) {
		const auto tmp = ResolvePath("Out.json");
		SymbolTable exported, imported;
		exported.Install("A", Structure{"A", "a", "namespace", StructureType::Class, "papas.cpp", 69, 420});
		ExportST(exported, tmp);

		ImportStashedST(tmp, imported);

		EXPECT_EQ(imported, exported) << DUMP_INFO(imported, exported);

		std::remove(tmp.data());
	}

	TEST(ImportStashedST, Imports_multiple_empty_structures_correctly) {
		const auto tmp = ResolvePath("Out.json");
		SymbolTable exported, imported;
		Repeat(URandom(2, 10), [i = 1, &exported]() mutable { 
			const auto id = "A" + std::to_string(i++);
			exported.Install(id, Structure{id, id, "namespace", StructureType::Class, "ballons.cpp", 69, 420});
		});
		ExportST(exported, tmp);

		ImportStashedST(tmp, imported);

		EXPECT_EQ(imported, exported) << DUMP_INFO(imported, exported);

		std::remove(tmp.data());
	}

} // namespace