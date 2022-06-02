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

		inline AccessType FromString(const JsonString& string) {
			using AccessTable = std::unordered_map<JsonString, AccessType>;

			static const auto table = AccessTable{ { "public", AccessType::_public },
			 									   { "protected", AccessType::_protected },
												   { "private", AccessType::_private },
												   { "unknown", AccessType::unknown } };

			assert(table.find(string) != std::end(table));

			return table.at(string);
		}

		template <typename TSymbol, typename Function>
		auto ImportSymbols(const JsonVal& val, SymbolTable& table, Function import) {
			static_assert(std::is_base_of_v<Symbol, TSymbol>, "Expected derivative of dependenciesMining::Symbol.");
			static_assert(std::is_invocable_r_v<TSymbol, Function, const JsonString&, const JsonVal&, SymbolTable&>, "Invalid import function.");

			using Symbols = std::vector<TSymbol>;

			Symbols syms;
			syms.reserve(std::distance(std::begin(val), std::end(val)));
			for (auto iter = std::begin(val); iter != std::end(val); ++iter) {
				auto sym = import(iter.name(), *iter, table);
				assert(table.Lookup(sym.GetID()));
				syms.push_back(std::move(sym));
			}

			return syms;
		}

		Definition ImportField(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			const auto access = Get(val, "access").asString();
			const auto type = Get(val, "type").asString();

			Definition field;
			field.SetID(id);
			field.SetAccessType(FromString(access));
			field.SetFullType(type);

			table.Install(id, field);

			return field;
		}

		inline auto DeserializeSrcInfo(const JsonVal& val) {
			const auto col = Get(val, "col").asInt();
			const auto file = Get(val, "file").asString();
			const auto line = Get(val, "line").asInt();

			return SourceInfo{file, line, col};
		}

		Structure ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			std::cout << id << '\n';

			Structure sym;
			sym.SetID(id);
			
			const auto contains = ImportSymbols<Structure>(Get(val, "contains"), table, ImportStructure);
			for (const auto& c : contains)
				sym.InstallNestedClass(c.GetID(), (Structure*) table.Lookup(c.GetID()));

			const auto fields = ImportSymbols<Definition>(Get(val, "fields"), table, ImportField);
			for (const auto& f : fields)
				sym.InstallField(f.GetID(), f);

			const auto friends = ImportSymbols<Structure>(Get(val, "friends"), table, ImportStructure);
			for (const auto& f : friends)
				sym.InstallFriend(f.GetID(), (Structure*) table.Lookup(f.GetID()));

			const auto srcInfo = DeserializeSrcInfo(Get(val, "src_info"));
			sym.SetSourceInfo(srcInfo);

			if(val.isMember("bases")) {
				assert(Get(val, "bases").isArray());

				for (const auto& val : Get(val, "bases")) {
					const auto base = val.asString();
					sym.InstallBase(base, (Structure*) table.Lookup(base));
				}
			}

			table.Install(sym.GetID(), sym);

			return sym;
		}

	}

	void ImportST(JsonArchive& from, SymbolTable& table) {
		assert(from.good());
		assert(from.is_open());

		const auto root = GetArchiveRoot(from);
		if(root) // valid json
			ImportSymbols<Structure>(Get(root, "structures"), table, ImportStructure);

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