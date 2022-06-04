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
			d->SetFullType(Get(val, "type").asString());
			d->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));
		}

		inline void DeserializeDefinitionWithAccessSpecifier(const SymbolID& id, const JsonVal& val, Definition* d) {
			DeserializeDefinition(id, val, d);
			d->SetAccessType(ToAccessType(Get(val, "access").asString()));
		}

		inline void DeserializeMethod(const SymbolID& id, const JsonVal& val, const SymbolTable& table, const Structure* s, Method* m) {
			m->SetID(id);
			m->SetName(id);
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
			m->SetNamespace(s->GetNamespace());
		}

		inline void DeserializeStructure(const SymbolID& id, const JsonVal& val, Structure* s) {
			s->SetID(id);
			s->SetName(id);
			s->SetNamespace(Get(val, "namespace").asString());
			s->SetStructureType(ToStructureType(Get(val, "structure_type").asString()));
			s->SetSourceInfo(DeserializeSrcInfo(Get(val, "src_info")));
		}

		Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table); // fwd declare for early usage

		inline void InstallMethodArg(const SymbolID& id, const JsonVal& val, Method* m) {
			assert(!m->GetArguments().Lookup(id));

			auto* d = (Definition*) m->InstallArg(id, {});
			assert(d);

			DeserializeDefinition(id, val, d);
		}

		inline void InstallMethodArgs(const JsonVal& val, Method* m) {
			ForEach(Get(val, "args"), [m](const auto& id, const auto& val) { 
				InstallMethodArg(id, val, m);
			});
		}

		inline void InstallMethodDefinition(const SymbolID& id, const JsonVal& val, Method* m) {
			assert(!m->GetDefinitions().Lookup(id));

			auto* d = (Definition*) m->InstallDefinition(id, {});
			assert(d);

			DeserializeDefinition(id, val, d);
		}

		inline void InstallMethodDefinitions(const JsonVal& val, Method* m) {
			ForEach(Get(val, "definitions"), [m](const auto& id, const auto& val) {
				InstallMethodDefinition(id, val, m);
			});
		}

		inline void ImportMethodTemplateArgs(const JsonVal& val, SymbolTable& table, Method* m) { // Why are we storing them like this?
			ForEach(Get(val, "template_args"), [m, &table](const auto& id, const auto& val) { 
				m->InstallTemplateSpecializationArgument(id, ImportStructure(id, val, table)); 
			});
		}

		inline void InstallStructureMethod(const SymbolID& id, const JsonVal& val, SymbolTable& table, Structure* s) {
			assert(!s->GetMethods().Lookup(id));

			auto* m = (Method*) s->InstallMethod(id, {});
			assert(m);

			DeserializeMethod(id, val,  table, s, m);

			InstallMethodArgs(val, m);
			InstallMethodDefinitions(val, m);
			ImportMethodTemplateArgs(val, table, m);
		}

		inline void ImportStructureNestedClasses(const JsonVal& val, SymbolTable& table, Structure* s) {
			ForEach(Get(val, "contains"), [&table, s](const auto& id, const auto& val) {
				s->InstallNestedClass(id, ImportStructure(id, val, table)); 
			});
		}

		inline void InstallStructureField(const SymbolID& id, const JsonVal& val, Structure* s) {
			assert(!s->GetFields().Lookup(id));

			auto* f = (Definition*) s->InstallField(id, {});
			assert(f);

			DeserializeDefinitionWithAccessSpecifier(id, val, f);

			f->SetType(s);
			f->SetNamespace(s->GetNamespace());
		}

		inline void InstallStructureFields(const JsonVal& val, Structure* s) {
			ForEach(Get(val, "fields"), [s](const auto& id, const auto& val) { 
				InstallStructureField(id, val, s);
			});
		}

		inline void ImportStructureFriends(const JsonVal& val, SymbolTable& table, Structure* s) {
			ForEach(Get(val, "friends"), [&table, s](const auto& id, const auto& val) {
				s->InstallFriend(id, ImportStructure(id, val, table)); 
			});
		}

		inline void InstallStructureMethods(const JsonVal& val, SymbolTable& table, Structure* s) {
			ForEach(Get(val, "methods"), [&table, s](const auto& id, const auto& val) {
				InstallStructureMethod(id, val, table, s);
			});
		}

		void InstallStructureBases(const JsonVal& val, const SymbolTable& table, Structure* s) {
			if (val.isMember("bases")) {
				assert(Get(val, "bases").isArray());

				for (const auto& base : Get(val, "bases")) {
					assert(base.isString());

					const auto id = base.asString();

					assert(!s->GetBases().Lookup(id));

					s->InstallBase(id, (Structure*) table.Lookup(id));
				}
			}
		}

		inline void InstalllStructureTemplateParent(const JsonVal& val, const SymbolTable& table, Structure* s) {
			if (val.isMember("template_parent"))
				s->SetTemplateParent((Structure*) table.Lookup(Get(val, "template_parent").asString()));
		}

		inline void InstalllStructureNestedParent(const JsonVal& val, const SymbolTable& table, Structure* s) {
			if (val.isMember("nested_parent"))
				s->SetNestedParent((Structure*) table.Lookup(Get(val, "nested_parent").asString()));
		}

		void InstallStructureTemplateArguments(const JsonVal& val, const SymbolTable& table, Structure* s) {
			if (val.isMember("template_args")) {
				assert(Get(val, "template_args").isArray());

				for (const auto& arg : Get(val, "template_args")) {
					assert(arg.isString());

					const auto id = arg.asString();

					assert(!s->GetTemplateArguments().Lookup(id));

					s->InstallTemplateSpecializationArgument(id, (Structure*) table.Lookup(id));
				}
			}
		}

		inline Structure* ImportStructure(const SymbolID& id, const JsonVal& val, SymbolTable& table) {
			auto* found = (Structure*) table.Lookup(id); // could already have been inserted (importing from bases, friend, etc.)
			if (found) // to avoid further deserialization
				return found; 

			auto* s = (Structure*) table.Install(id, Structure{});
			assert(s);

			DeserializeStructure(id, val, s);

			ImportStructureNestedClasses(val, table, s);
			InstallStructureFields(val, s);
			ImportStructureFriends(val, table, s);
			InstallStructureMethods(val, table, s);

			InstallStructureBases(val, table, s);
			InstalllStructureTemplateParent(val, table, s);
			InstalllStructureNestedParent(val, table, s);
			InstallStructureTemplateArguments(val, table, s);

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