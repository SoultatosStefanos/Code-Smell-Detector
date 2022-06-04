// Contains functions to import a Symbol Table from a json file.
// Soultatos Stefanos 2022

#include "ImportST.h"
#include "Converters.h"
#include <cassert>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <filesystem>
#include <algorithm>
#include <sstream>

// TODO Deserialize rest of symbols

namespace incremental {
	
	using namespace details;

	namespace {

		using JsonVal = Json::Value;
		using JsonString = Json::String;
		using SymbolID = JsonString;
		using namespace dependenciesMining;

		inline JsonVal GetArchiveRoot(JsonArchive& from) {
			JsonVal root;
			from >> root;
			return root;
		}

		// Utility for safe json access.
		inline auto Get(const JsonVal& val, const char* at) { 
			assert(val.isMember(at));
			return val[at];
		}

		// Utility for id based json iteration.
		template <typename Function>
		void ForEach(const JsonVal& val, Function f) {
			static_assert(std::is_invocable_v<Function, const JsonString&, const JsonVal&>, "Invalid function.");

			for (auto iter = std::begin(val); iter != std::end(val); ++iter)
				f(iter.name(), *iter);
		}

		inline SourceInfo DeserializeSrcInfo(const JsonVal& val) {
			return { Get(val, "file").asString(), Get(val, "line").asInt(), Get(val, "col").asInt() };
		}

		inline void DeserializeDefinition(const SymbolID& id, const JsonVal& val, Definition* d) {
			d->SetID(id);
			d->SetName(id);
			if (val.isMember("access")) // cause of non member definitions
				d->SetAccessType(ToAccessType(Get(val, "access").asString()));
			d->SetFullType(Get(val, "type").asString());
			d->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));
		}


		inline Definition* ImportDefinition(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* d = (Definition*) table.Install(id, Definition{});
			assert(d);

			DeserializeDefinition(id, val ,d);

			return d;
		}

		Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table); // fwd declare for early usage

		inline void DeserializeMethod(const SymbolID& id, const JsonVal& val, SymbolTable& table, Method* m) {
			m->SetID(id);
			m->SetBranches(Get(val, "branches").asInt());
			m->SetLineCount(Get(val, "lines").asInt());
			m->SetLiterals(Get(val, "literals").asInt());
			m->SetLoops(Get(val, "loops").asInt());
			m->SetMaxScopeDepth(Get(val, "max_scope").asInt());
			m->SetReturnType( (Structure*) table.Lookup(Get(val, "ret_type").asString()) ); // will write nullptr in case of "void"
			m->SetAccessType(ToAccessType(Get(val, "access").asString()));
			m->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));
			m->SetStatements(Get(val, "statements").asInt());
			m->SetVirtual(Get(val, "virtual").asBool());
			m->SetMethodType(ToMethodType(Get(val, "method_type").asString()));
		}

		inline void ImportArgs(const JsonVal& val, SymbolTable& table, Method* m) {
			ForEach(Get(val, "args"), [m, &table](const auto& id, const auto& val) { 
				auto* arg = ImportDefinition(id, val, table);
				assert(arg);

				m->InstallArg(id, *arg); 
			});
		}

		inline void ImportDefinitions(const JsonVal& val, SymbolTable& table, Method* m) {
			ForEach(Get(val, "definitions"), [m, &table](const auto& id, const auto& val) {
				auto* def = ImportDefinition(id, val, table);
				assert(def);

				m->InstallDefinition(id, *def); 
			});
		}

		inline void ImportTemplateArgs(const JsonVal& val, SymbolTable& table, Method* m) {
			ForEach(Get(val, "template_args"), [m, &table](const auto& id, const auto& val) { 
				auto* s = ImportStructure(id, val, table);
				assert(s);

				m->InstallTemplateSpecializationArguments(id, s); 
			});
		}

		inline Method* ImportMethod(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* m = (Method*) table.Install(id, Method{});
			assert(m);

			DeserializeMethod(id, val,  table, m);

			ImportArgs(val, table, m);
			ImportDefinitions(val, table, m);
			ImportTemplateArgs(val, table, m);

			return m;
		}

		// TODO Optimize on Install

		inline void DeserializeStructure(const SymbolID& id, const JsonVal& val, Structure* s) {
			s->SetID(id);
			s->SetName(id);
			s->SetNamespace(Get(val, "namespace").asString());
			s->SetStructureType(ToStructureType(Get(val, "structure_type").asString()));
			s->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));
		}

		inline void ImportNestedClasses(const JsonVal& val, SymbolTable& table, Structure* s) {
			ForEach(Get(val, "contains"), [&table, s](const auto& id, const auto& val) { 
				auto* c = ImportStructure(id, val, table);
				assert(c);

				s->InstallNestedClass(id, c); 
			});
		}

		inline void ImportFields(const JsonVal& val, SymbolTable& table, Structure* s) {
			ForEach(Get(val, "fields"), [&table, s](const auto& id, const auto& val) { 
				auto* f = ImportDefinition(id, val, table);
				assert(f);

				f->SetType(s);
				f->SetNamespace(s->GetNamespace());
				s->InstallField(id, *f);
			});
		}

		inline void ImportFriends(const JsonVal& val, SymbolTable& table, Structure* s) {
			ForEach(Get(val, "friends"), [&table, s](const auto& id, const auto& val) {
				auto* f = ImportStructure(id, val, table);
				assert(f);

				s->InstallFriend(id, f); 
			});
		}

		inline void ImportMethods(const JsonVal& val, SymbolTable& table, Structure* s) {
			ForEach(Get(val, "methods"), [&table, s](const auto& id, const auto& val) {
				auto* m = ImportMethod(id, val, table);
				assert(m);

				m->SetNamespace(s->GetNamespace());
				s->InstallMethod(id, *m);
			});
		}

		void InstallBases(const JsonVal& val, SymbolTable& table, Structure* s) {
			if (val.isMember("bases")) {
				assert(Get(val, "bases").isArray());

				for (const auto& base : Get(val, "bases")) {
					assert(base.isString());

					const auto id = base.asString();
					s->InstallBase(id, (Structure*) table.Lookup(id));
				}
			}
		}

		inline void InstalllTemplateParent(const JsonVal& val, SymbolTable& table, Structure* s) {
			if (val.isMember("template_parent"))
				s->SetTemplateParent((Structure*) table.Lookup(Get(val, "template_parent").asString()));
		}

		inline void InstalllNestedParent(const JsonVal& val, SymbolTable& table, Structure* s) {
			if (val.isMember("nested_parent"))
				s->SetNestedParent((Structure*) table.Lookup(Get(val, "nested_parent").asString()));
		}

		void InstallTemplateArguments(const JsonVal& val, SymbolTable& table, Structure* s) {
			if (val.isMember("template_args")) {
				assert(Get(val, "template_args").isArray());

				for (const auto& arg : Get(val, "template_args")) {
					assert(arg.isString());

					const auto id = arg.asString();
					s->InstallTemplateSpecializationArguments(id, (Structure*) table.Lookup(id));
				}
			}
		}

		inline Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* s = (Structure*) table.Install(id, Structure{});
			assert(s);

			DeserializeStructure(id, val, s);

			ImportNestedClasses(val, table, s);
			ImportFields(val, table, s);
			ImportFriends(val, table, s);
			ImportMethods(val, table, s);

			InstallBases(val, table, s);
			InstalllTemplateParent(val, table, s);
			InstalllNestedParent(val, table, s);
			InstallTemplateArguments(val, table, s);

			return s;
		}

		inline void ImportStructures(const JsonVal& val, SymbolTable& table) {
			ForEach(Get(val, "structures"), [&table](const auto& id, const auto& val) { 
				ImportStructure(id, val, table); 
			});
		}

	} // namespace

	void ImportST(JsonArchive& from, SymbolTable& table) {
		assert(from.good());
		assert(from.is_open());

		ImportStructures(GetArchiveRoot(from), table);

		assert(from.good());
		assert(from.is_open());
	}

	void ImportStashedST(const std::string_view fpath, dependenciesMining::SymbolTable& table) {
		if (std::filesystem::exists(fpath)) {
			JsonArchive file{fpath.data()};
			ImportST(file, table);
		}
	}

} // incremental