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

		inline SourceInfo DeserializeSrcInfo(const JsonVal& val) {
			return { Get(val, "file").asString(), Get(val, "line").asInt(), Get(val, "col").asInt() };
		}

		// Utility for id based json iteration
		template <typename Function>
		void ForEach(const JsonVal& val, Function f) {
			static_assert(std::is_invocable_v<Function, const JsonString&, const JsonVal&>, "Invalid function.");
			for (auto iter = std::begin(val); iter != std::end(val); ++iter)
				f(iter.name(), *iter);
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


		Definition* ImportDefinition(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* f = (Definition*) table.Install(id, Definition{});
			assert(f);

			f->SetID(id);
			if (val.isMember("access"))
				f->SetAccessType(FromString(Get(val, "access").asString()));
			f->SetFullType(Get(val, "type").asString());

			return f;
		}

		Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table);

		Method* ImportMethod(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* m = (Method*) table.Install(id, Method{});
			assert(m);

			m->SetID(id);
			m->SetBranches(Get(val, "branches").asInt());
			m->SetLineCount(Get(val, "lines").asInt());
			m->SetLiterals(Get(val, "literals").asInt());
			m->SetLoops(Get(val, "loops").asInt());
			m->SetMaxScopeDepth(Get(val, "max_scope").asInt());
			m->SetReturnType( (Structure*) table.Lookup(Get(val, "ret_type").asString()) ); // FIXME?
			m->SetAccessType(FromString(Get(val, "access").asString()));
			m->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));
			m->SetStatements(Get(val, "statements").asInt());
			m->SetVirtual(Get(val, "virtual").asBool());

			// Import args.
			ForEach(Get(val, "args"), [m, &table](const auto& id, const auto& val) { 
				auto* arg = ImportDefinition(id, val, table);
				assert(arg);

				m->InstallArg(id, *arg); 
			});

			// Import definitions.
			ForEach(Get(val, "definitions"), [m, &table](const auto& id, const auto& val) {
				auto* def = ImportDefinition(id, val, table);
				assert(def);

				m->InstallDefinition(id, *def); 
			});

			// Import template args.
			ForEach(Get(val, "template_args"), [m, &table](const auto& id, const auto& val) { 
				auto* s = ImportStructure(id, val, table);
				assert(s);

				m->InstallTemplateSpecializationArguments(id, s); 
			});

			return m;
		}

		Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* s = (Structure*) table.Install(id, Structure{});
			assert(s);

			s->SetID(id);
			s->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));

			// Import nested classes.
			ForEach(Get(val, "contains"), [&table, s](const auto& id, const auto& val) { 
				auto* c = ImportStructure(id, val, table);
				assert(c);

				s->InstallNestedClass(id, c); 
			});

			// Import fields.
			ForEach(Get(val, "fields"), [&table, s](const auto& id, const auto& val) { 
				auto* f = ImportDefinition(id, val, table);
				assert(f);

				f->SetType(s);
				s->InstallField(id, *f);
			});

			// Import friends.
			ForEach(Get(val, "friends"), [&table, s](const auto& id, const auto& val) {
				auto* f = ImportStructure(id, val, table);
				assert(f);

				s->InstallFriend(id, f); 
			});

			// Import methods.
			ForEach(Get(val, "methods"), [&table, s](const auto& id, const auto& val) {
				auto* m = ImportMethod(id, val, table);
				assert(m);

				s->InstallMethod(id, *m);
			});

			// Import bases.
			if(val.isMember("bases")) { 	// could not be at json file
				assert(Get(val, "bases").isArray());

				for (const auto& base : Get(val, "bases")) {
					assert(base.isString());

					const auto id = base.asString();
					s->InstallBase(id, (Structure*) table.Lookup(id));
				}
			}

			return s;
		}

	}

	void ImportST(JsonArchive& from, SymbolTable& table) {
		assert(from.good());
		assert(from.is_open());

		const auto root = GetArchiveRoot(from);
		if(root)
			ForEach(Get(root, "structures"), [&table](const auto& id, const auto& val) { 
				ImportStructure(id, val, table); 
			});

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