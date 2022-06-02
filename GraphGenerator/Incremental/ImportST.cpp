#include "ImportST.h"
#include <cassert>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <filesystem>
#include <algorithm>
#include <sstream>

namespace incremental {

	namespace {

		using JsonVal = Json::Value;
		using JsonString = Json::String;
		using SymbolID = JsonString;
		using namespace dependenciesMining;

		inline JsonVal GetArchiveRoot(JsonArchive& from) {
			JsonVal root;
			try {				// TODO Cleanup, currently cannot check for file validity elsewise
				from >> root;
			} catch(Json::RuntimeError& err) {}
			return root;
		}

		inline auto Get(const JsonVal& val, const char* at) { // safe access
			assert(val.isMember(at));
			return val[at];
		}

		Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table);

		void ImportStructures(const JsonVal& val, SymbolTable& table) {
			for (auto iter = std::begin(val); iter != std::end(val); ++iter)
				ImportStructure(iter.name(), *iter, table);
		}

		template <typename Function>
		void ImportStructureSymbols(const JsonVal& val, SymbolTable& table, Structure* s, Function import) {
			static_assert(std::is_invocable_v<Function, const JsonString&, const JsonVal&, SymbolTable&, Structure*>, "Invalid import function.");

			assert(s);

			for (auto iter = std::begin(val); iter != std::end(val); ++iter)
				import(iter.name(), *iter, table, s);
		}
		
		inline AccessType FromString(const JsonString& string) {
			using AccessTable = std::unordered_map<JsonString, AccessType>;

			static const auto table = AccessTable{ { "public", AccessType::_public },
			 									   { "protected", AccessType::_protected },
												   { "private", AccessType::_private },
												   { "unknown", AccessType::unknown } };

			assert(table.find(string) != std::end(table));

			return table.at(string);
		}

		void ImportField(const SymbolID& id, const JsonVal& val, SymbolTable& table, Structure* s) {
			assert(s);

			auto* f = (Definition*) table.Install(id, Definition{});
			assert(f);

			f->SetID(id);
			f->SetAccessType(FromString(Get(val, "access").asString()));
			f->SetFullType(Get(val, "type").asString());
			f->SetType(s);

			s->InstallField(id, *f);
		}

		inline void ImportNestedClass(const SymbolID& id, const JsonVal& val, SymbolTable& table, Structure* s) {
			assert(s);

			auto* c = ImportStructure(id, val, table);
			assert(c);

			s->InstallNestedClass(id, c);
		}

		inline void ImportFriend(const SymbolID& id, const JsonVal& val, SymbolTable& table, Structure* s) {
			assert(s);

			auto* f = ImportStructure(id, val, table);
			assert(f);

			s->InstallFriend(id, f);
		}

		void ImportBases(const JsonVal& val, SymbolTable& table, Structure* s) {
			assert(s);
			assert(val.isArray());

			for (const auto& base : val) {
				assert(base.isString());

				const auto id = base.asString();
				s->InstallBase(id, (Structure*) table.Lookup(id));
			}
		}

		inline SourceInfo DeserializeSrcInfo(const JsonVal& val) {
			return { Get(val, "file").asString(), Get(val, "line").asInt(), Get(val, "col").asInt() };
		}

		Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* s = (Structure*) table.Install(id, Structure{});

			assert(s);

			s->SetID(id);
			s->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));

			ImportStructureSymbols(Get(val, "contains"), table, s, ImportNestedClass);
			ImportStructureSymbols(Get(val, "fields"), table, s, ImportField);
			ImportStructureSymbols(Get(val, "friends"), table, s, ImportFriend);

			if(val.isMember("bases")) // could not be at json file
				ImportBases(Get(val, "bases"), table, s);

			return s;
		}

	}

	void ImportST(JsonArchive& from, SymbolTable& table) {
		assert(from.good());
		assert(from.is_open());

		const auto root = GetArchiveRoot(from);
		if(root) // valid json
			ImportStructures(Get(root, "structures"), table);

		assert(from.good());
		assert(from.is_open());
	}

	void ImportSTIfCached(const std::string_view fpath, dependenciesMining::SymbolTable& table) {
		if (std::filesystem::exists(fpath)) {
			JsonArchive file{fpath.data()};
			ImportST(file, table);
		}
	}

}