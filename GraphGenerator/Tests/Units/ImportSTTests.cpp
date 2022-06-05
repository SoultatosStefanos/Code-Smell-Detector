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

	// --------------------- sample_non_empty.json hardcoded ----------------- //

	Structure* PrepareStructure(SymbolTable& expected) {
		auto* s = (Structure*) expected.Install("CS::CS_1::class_A", Structure{});
		assert(s);

		s->SetID("CS::CS_1::class_A");
		s->SetName("CS::CS_1::class_A");
		s->SetNamespace("CS::CS_1::");
		s->SetSourceInfo({"classes_simple.cpp", 11, 11});
		s->SetStructureType(StructureType::Class);

		return s;
	}

	void PrepareField(SymbolTable& expected, Structure* s) {
		assert(s);

		auto* f = (Definition*) s->InstallField("CS::CS_1::class_A::b", {});
		assert(f);

		f->SetID("CS::CS_1::class_A::b");
		f->SetName("CS::CS_1::class_A::b");
		f->SetNamespace(s->GetNamespace());
		f->SetSourceInfo({"classes_simple.cpp", 34, 2});
		f->SetAccessType(AccessType::_private);
		f->SetFullType("class CS::CS_1::class_B");
		f->SetType(s);
	}

	void PrepareMethod(SymbolTable& expected, Structure* s) {
		assert(s);

		auto* m = (Method*) s->InstallMethod("CS::CS_1::class_A::class_A(class CS::CS_1::class_A &&)", {});
		assert(m);

		m->SetID("CS::CS_1::class_A::class_A(class CS::CS_1::class_A &&)");
		m->SetName("CS::CS_1::class_A::class_A(class CS::CS_1::class_A &&)");
		m->SetNamespace(s->GetNamespace());
		m->SetSourceInfo({"classes_simple.cpp", 11, 11});
		m->SetAccessType(AccessType::unknown);
		m->SetBranches(0);
		m->SetLineCount(0);
		m->SetLiterals(0);
		m->SetLoops(0);
		m->SetMaxScopeDepth(0);
		m->SetStatements(0);
		m->SetVirtual(false);
		m->SetMethodType(MethodType::Constructor_Trivial);
	}

	void Prepare(SymbolTable& expected) {
		auto* s = PrepareStructure(expected);
		PrepareField(expected, s);
		PrepareMethod(expected, s);
	}

	// -------------------------------------------------------------------- //

	TEST(ImportStashedST, Imports_non_empty_symbol_table_correctly) {
		const auto path = RESOLVE_PATH("sample_non_empty.json");
		assert(std::filesystem::exists(path));
		SymbolTable actual, expected;
		Prepare(expected);

		ImportStashedST(path, actual);

		ASSERT_TRUE(AreEqual(expected, actual));
	}

} // namespace