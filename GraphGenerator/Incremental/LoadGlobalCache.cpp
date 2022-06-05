// Contains an adaptor function for incremental generation.
// Soultatos Stefanos 2022

#include "LoadGlobalCache.h"

namespace incremental {

	using namespace dependenciesMining;

	namespace {

		void LoadDefinitions(const SymbolTable& from, SymbolTable& to) {
			for (const auto& [id, symbol] : from) 
				to.Install(id, symbol);
		}

		void LoadMethods(const SymbolTable& from, SymbolTable& to) {
			for (const auto& [id, method] : from) {
				assert(method->GetClassType() == ClassType::Method);

				auto* m = (Method*) to.Install(id, method);
				assert(m);

				LoadDefinitions(m->GetArguments(), to);
				LoadDefinitions(m->GetDefinitions(), to);
			} 
		}

		void LoadStructures(const SymbolTable& from, SymbolTable& to) {
			for (const auto& [id, structure] : from) {
				assert(structure->GetClassType() == ClassType::Structure);

				auto* s = (Structure*) to.Install(id, structure);
				assert(s);

				LoadMethods(s->GetMethods(), to);
				LoadDefinitions(s->GetFields(), to);
				LoadStructures(s->GetContains(), to);
			} 
		}


	} // namespace

	void LoadGlobalCache(const SymbolTable& from, SymbolTable& to) {
		LoadStructures(from, to);
	}


} // incremental