#include "ImportST.h"
#include <cassert>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <filesystem>

namespace incremental {

	namespace {

		using JsonVal = Json::Value;
		using JsonCIter = JsonVal::const_iterator;
		using dependenciesMining::SymbolTable;

		JsonVal GetArchiveRoot(JsonArchive& from) {
			JsonVal root;
			from >> root;
			return root;
		}

		void ImportStructure(JsonCIter s,  SymbolTable& cache);

		void ImportField(JsonCIter s,  SymbolTable& cache) {}

		void ImportFriend(JsonCIter s,  SymbolTable& cache) {}

		void ImportMethod(JsonCIter s,  SymbolTable& cache) {}

		void ImportSrcInfo(const JsonVal& s,  SymbolTable& cache) {}

		void ImportStructures(const JsonVal& structures,  SymbolTable& cache) {
			for (auto iter = structures.begin(); iter != structures.end(); ++iter)
				ImportStructure(iter, cache);
		}

		void ImportFields(const JsonVal& fields,  SymbolTable& cache) {
			for (auto iter = fields.begin(); iter != fields.end(); ++iter)
				ImportField(iter, cache);
		}

		void ImportFriends(const JsonVal& friends,  SymbolTable& cache) {
			for (auto iter = friends.begin(); iter != friends.end(); ++iter)
				ImportFriend(iter, cache);
		}

		void ImportMethods(const JsonVal& methods,  SymbolTable& cache) {
			for (auto iter = methods.begin(); iter != methods.end(); ++iter)
				ImportMethod(iter, cache);
		}

		void ImportStructure(JsonCIter s,  SymbolTable& cache) {
			std::cout << s.name() << '\n';
			ImportStructures((*s)["contains"], cache);
			ImportFields((*s)["fields"], cache);
			ImportFriends((*s)["friends"], cache);
			ImportMethods((*s)["methods"], cache);
			ImportSrcInfo((*s)["src_info"], cache);
		}

	}

	void ImportST(JsonArchive& from, SymbolTable& cache) {
		assert(from.good());
		assert(from.is_open());

		const auto root = GetArchiveRoot(from);
		ImportStructures(root["structures"], cache);

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